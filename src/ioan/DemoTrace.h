#ifndef __choco_xcode__DemoTrace__
#define __choco_xcode__DemoTrace__

#include "ArrayList.h"
#include "../doom/doomdef.h"
#include "../m_fixed.h"

struct mobj_s;
struct player_s;

typedef struct PlayerPos
{
    fixed_t x, y;
    unsigned armed;
} PlayerPos;

ArrayList dt_playerPos[MAXPLAYERS];
ArrayList dt_itemPos;

void DT_ResetTrace(void);

void DT_AppendPlayerPos(const struct player_s* pl, boolean armed);
void DT_AppendItemPos(const struct mobj_s* mo);

#endif /* defined(__choco_xcode__DemoTrace__) */
