#include "DemoTrace.h"
#include "../doom/doomstat.h"
#include "../doom/d_player.h"

void DT_ResetTrace(void)
{
    for(int i = 0; i < MAXPLAYERS; ++i)
        ArrayList_Init(dt_playerPos + i, sizeof(PlayerPos), 1024);
    ArrayList_Init(&dt_itemPos, sizeof(PlayerPos), 1024);
}

void DT_AppendPlayerPos(const player_t* pl, boolean armed)
{
    ArrayList* al = &dt_playerPos[pl - players];
    PlayerPos npp;
    npp.x = pl->mo->x;
    npp.y = pl->mo->y;
    npp.armed = armed ? 1 : 0;
    if(al->nelts)
    {
        const PlayerPos* pp = al->elts;
        pp += al->nelts - 1;
        if(pp->x == npp.x && pp->y == npp.y)
            return;
    }
    
    ArrayList_Add(al, &npp);
}

void DT_AppendItemPos(const mobj_t* mo)
{
    PlayerPos npp;
    npp.x = mo->x;
    npp.y = mo->y;
    npp.armed = 0;
    ArrayList_Add(&dt_itemPos, &npp);
}