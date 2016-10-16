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

#ifndef RC_MAIN_H_
#define RC_MAIN_H_

#ifdef FEATURE_REMOTE_CONTROL

#include "doomtype.h"

boolean RC_Init(void);
void RC_Shutdown(void);
    
#endif


#endif
