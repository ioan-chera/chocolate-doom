//
// Copyright(C) 2016 Ioan Chera
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
//
// Remote control interprocess server. It sends image and sound data to clients
// and receives mouse and keyboard input.
//

#include "doomfeatures.h"

#ifdef FEATURE_REMOTE_CONTROL

#include <errno.h>

#ifdef _WIN32
#error not implemented in Windows
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <unistd.h>
#endif

#include "SDL_thread.h"

#include "m_misc.h"
#include "rc_main.h"

#define RC_SERVER_NAME "choco-doom-remote-control"  // name of server
#define RC_RETRY_DOWNTIME_MS 10000  // how long to wait in case of serious error

#define RC_ERROR_FAILURE 1          // Thread failure

//
// Internal pipe opcodes
//
typedef enum rc_opcode_e
{
    RC_OPCODE_QUIT                  // quit the thread
} rc_opcode_t;

static SDL_Thread *rc_thread;       // thread handle

#ifdef _WIN32
#error not implemented in Windows
#else
static int rc_socket = -1;          // server socket
static int rc_pipe[2] = { -1, -1 }; // inter-thread communication

static fd_set rc_fds;               // select() master read set
static int rc_fdmax = -1;           // select() read set maximum

static void RC_addToFdSet(int fd)
{
    FD_SET(fd, &rc_fds);
    if (rc_fdmax > fd)
    {
        rc_fdmax = fd;
    }
}

static void RC_removeFromFdSet(int fd)
{
    FD_CLR(fd, &rc_fds);
    if (rc_fdmax == fd)
    {
        int i;
        for (i = fd - 1; i >= 0; --i)
        {
            if (FD_ISSET(i, &rc_fds))
            {
                rc_fdmax = i;
                return;
            }
        }
        rc_fdmax = -1;  // none found, so reset it
    }
}

#endif

//
// The thread routine
//
static int RC_threadFunc(void *data)
{
#ifdef _WIN32
#error not implemented in Windows
#else
    fd_set read_fds;
    int sel_res;
    boolean quit = false;

    while (!quit)
    {
        int fd_iter;
        int so_far_count;

        read_fds = rc_fds;
        sel_res = select(rc_fdmax + 1, &read_fds, NULL, NULL, NULL);

        if (sel_res == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                return RC_ERROR_FAILURE;  // thread will fail
            }
        }

        so_far_count = 0;
        for (fd_iter = 0; fd_iter <= rc_fdmax && so_far_count < sel_res;
             ++fd_iter)
        {
            if (!FD_ISSET(fd_iter, &read_fds))
            {
                continue;
            }
            ++so_far_count;
            if (fd_iter == rc_pipe[0])
            {
                // read from a pipe
                rc_opcode_t opcode;
                ssize_t size = read(rc_pipe[0], &opcode, sizeof(opcode));
                if (size != sizeof(opcode))
                {
                    continue;   // errors and bad messages are ignored
                }
                if (opcode == RC_OPCODE_QUIT)
                {
                    // just exit it
                    return 0;
                }
                continue;
            }
            if (fd_iter == rc_socket)
            {
                // new match
                struct sockaddr_un remote;
                socklen_t len = sizeof(remote);
                int new_sock = accept(rc_socket, &remote, &len);
                if (new_sock != -1)
                {
                    // Add it to set if valid
                    RC_addToFdSet(new_sock);
                }
                continue;
            }
            // Otherwise it's a client. Read its command
            // TODO
        }
    }

    // Clean-up will be done from outside

#endif
    return 0;
}

//
// Conveniently closes both pipes
//
static void RC_closePipe(void)
{
#ifdef _WIN32
#error not implemented in Windows
#else
    if (rc_pipe[0] != -1)
    {
        close(rc_pipe[0]);
        rc_pipe[0] = -1;
    }
    if (rc_pipe[1] != -1)
    {
        close(rc_pipe[1]);
        rc_pipe[1] = -1;
    }
#endif
}

//
// Creates the server (e.g. socket server under POSIX)
//
static boolean RC_createServer(void)
{
#ifdef _WIN32
#error not implemented for Windows
#else
    struct sockaddr_un local;
    socklen_t local_len;

    if (pipe(rc_pipe) == -1)
    {
        return false;
    }

    rc_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (rc_socket == -1)
    {
        RC_closePipe();
        return false;
    }


    memset(&local, 0, sizeof(local));
    local.sun_family = AF_UNIX;
#ifdef __APPLE__
#error not implemented on macOS
#else
    M_StringCopy(local.sun_path + 1, RC_SERVER_NAME,
                 sizeof(local.sun_path) - 1);
#endif

    local_len = offsetof(struct sockaddr_un, sun_path)
        + sizeof(RC_SERVER_NAME);  // this includes the null separator

    if (bind(rc_socket, (struct sockaddr *)&local, local_len) == -1)
    {
        close(rc_socket);
        rc_socket = -1;
        RC_closePipe();
        return false;
    }

    if (listen(rc_socket, SOMAXCONN) == -1)
    {
        close(rc_socket);
        rc_socket = -1;
        RC_closePipe();
        return false;
    }

    RC_addToFdSet(rc_socket);
    RC_addToFdSet(rc_pipe[0]);

#endif

    return true;
}

//
// Initializes the remote-control subsystem, starting a new thread if succeeded.
// It waits for the new thread to finish initialization before exiting.
// The thread is controlled via SDL.
//
boolean RC_Init(void)
{
    if (rc_thread)
    {
        return false;   // error if initialized several times
    }

    // Create server here first
    if (!RC_createServer())
    {
        return false;
    }
    // Start a new thread
    rc_thread = SDL_CreateThread(RC_threadFunc, NULL);
    if (!rc_thread)
    {
        return false;
    }

    return true;
}

//
// Safely writes a packet to a certainly existing file descriptor, handling any
// EINTR cases
//
static void RC_safeWrite(int fd, void *msg, size_t size)
{
    ssize_t s;
    do
    {
        s = write(fd, msg, size);
    } while (s == -1 && errno == EINTR);
}

// Cleans up resources (may be plat dependent)
static void RC_cleanUp(void)
{
#ifdef _WIN32
#error not implemented in Windows
#else
    int i;
    for (i = 0; i <= rc_fdmax; ++i)
    {
        if (FD_ISSET(i, &rc_fds))
        {
            close(i);
        }
    }

    FD_ZERO(&rc_fds);
    rc_fdmax = -1;
    rc_socket = -1;
    RC_closePipe();
#endif
}

//
// Shuts down the remote-control subsystem by telling the thread to quit and
// cleaning up resources
//
void RC_Shutdown(void)
{
    rc_opcode_t opcode = RC_OPCODE_QUIT;
    int status = 0;

    if (!rc_thread)
    {
        return;
    }

#ifdef _WIN32
#error not implemented in Windows
#else
    RC_safeWrite(rc_pipe[1], &opcode, sizeof(opcode));
#endif

    SDL_WaitThread(rc_thread, &status);
    rc_thread = NULL;

    RC_cleanUp();
}

#if 0

#include <errno.h>
#include <stddef.h>
#include <string.h>

#include <vector>

#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "BotAvStreamer.h"

using std::vector;

static int pipes[2] = {-1, -1};

static const void *frame;

static void closepipes()
{
    if(pipes[0] != -1)
        close(pipes[0]);
    if(pipes[1] != -1)
        close(pipes[1]);
}

struct Client
{
    explicit Client(int nn) : n(nn) {}
    int n;
};

class Server
{
public:
    Server() : fds(), fdmax(-1)
    {
    }
    ~Server()
    {
        for(int c : clients)
            close(c);
    }
    Server &operator << (int fd)
    {
        FD_SET(fd, &fds);
        if(fdmax > fd)
            fdmax = fd;
        return *this;
    }
    Server &operator << (Client c)
    {
        *this << c.n;
        clients.push_back(c.n);
        return *this;
    }
    bool select(vector<int> &items) const
    {
        fd_set ret = fds;
        int result = ::select(fdmax + 1, &ret, nullptr, nullptr, nullptr);
        if(result == -1)
        {
            if(errno == EINTR)
                return select(items);
            return false;
        }
        items.clear();
        items.reserve(fdmax + 1);
        for(int i = 0; i <= fdmax; ++i)
        {
            if(FD_ISSET(i, &ret))
                items.push_back(i);
        }
        return true;
    }
    void end(int fd)
    {
        close(fd);
        for(auto it = clients.begin(); it != clients.end(); ++it)
        {
            if(*it == fd)
            {
                *it = *(clients.end() - 1);
                clients.pop_back();
            }
        }
        if(fd == fdmax)
        {
            int i;
            for(i = fdmax - 1; i >= 0; --i)
            {
                if(FD_ISSET(i, &fds))
                {
                    fdmax = i;
                    break;
                }
            }
            if(i == -1)
                fdmax = -1;
        }
    }
    void bcast(char b[320*200]) const
    {
        for(int c : clients)
        {
            write(c, b, 320*200);
        }
    }
private:
    vector<int> clients;
    fd_set fds;
    int fdmax;
};

bool BAS_Start(const char *name, const void *frame320x200)
{
    frame = frame320x200;
    auto local = sockaddr_un();
    if(!name || strlen(name) + 1 < sizeof(local.sun_path))
        return false;

    if(pipe(pipes) == -1)
        return false;

    // Create UNIX socket
    int s = socket(AF_UNIX, SOCK_STREAM, 0);
    if(s == -1)
    {
        closepipes();
        return false;
    }

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, name);
    if(unlink(local.sun_path) == -1)
    {
        switch(errno)
        {
            case ENOENT:
                break;
            default:
                close(s);
                closepipes();
                return false;
        }
    }
    auto len = static_cast<socklen_t>(offsetof(sockaddr_un, sun_path) +
                                      sizeof(local.sun_path));

    if(bind(s, reinterpret_cast<sockaddr *>(&local), len) == -1)
    {
        close(s);
        closepipes();
        return false;
    }
    if(listen(s, SOMAXCONN) == -1)
    {
        close(s);
        closepipes();
        return false;
    }

    Server srv;
    srv << s << pipes[0];

    vector<int> fds;
    bool getout = false;
    while(!getout && srv.select(fds))
    {
        for(int fd : fds)
        {
            if(fd == s)
            {
                auto remote = sockaddr_un();
                socklen_t len = sizeof(remote);
                int news = accept(s, reinterpret_cast<sockaddr *>(&remote), &len);
                if(news != -1)
                    srv << Client(news);
            }
            else if(fd == pipes[0])
            {
                char opcode;
                read(pipes[0], &opcode, 1);
                switch(static_cast<BAS_Opcode>(opcode))
                {
                    case BAS_Opcode_quit:
                        getout = true;
                        break;
                }
                if(getout)
                    break;
            }
            else
            {
                // a client
                static char dummy;
                ssize_t r = read(fd, &dummy, 1);
                if(r == 0)    // client disconnected
                {
                    srv.end(fd);
                }
                else if(r < 0 && errno != EINTR)
                {
                    srv.end(fd);
                }
                else if(r >= 1)
                {
                    write(fd, frame, 320*200);
                }
            }
        }
    }

    close(s);
    closepipes();

    return false;
}

void BAS_ShutDown(void)
{
    auto msg = static_cast<char>(BAS_Opcode_quit);
    if(pipes[1] >= 0)
        write(pipes[1], &msg, 1);
}
#endif
#endif
