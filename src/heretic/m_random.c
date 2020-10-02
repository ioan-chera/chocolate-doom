//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 1993-2008 Raven Software
// Copyright(C) 2005-2014 Simon Howard
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

#include "d_loop.h"
#include "m_random.h"

/*
===============
=
= M_Random
=
= Returns a 0-255 number
=
===============
*/

const unsigned int rndtable[256] = {
    0, 8, 109, 220, 222, 241, 149, 107, 75, 248, 254, 140, 16, 66,
    74, 21, 211, 47, 80, 242, 154, 27, 205, 128, 161, 89, 77, 36,
    95, 110, 85, 48, 212, 140, 211, 249, 22, 79, 200, 50, 28, 188,
    52, 140, 202, 120, 68, 145, 62, 70, 184, 190, 91, 197, 152, 224,
    149, 104, 25, 178, 252, 182, 202, 182, 141, 197, 4, 81, 181, 242,
    145, 42, 39, 227, 156, 198, 225, 193, 219, 93, 122, 175, 249, 0,
    175, 143, 70, 239, 46, 246, 163, 53, 163, 109, 168, 135, 2, 235,
    25, 92, 20, 145, 138, 77, 69, 166, 78, 176, 173, 212, 166, 113,
    94, 161, 41, 50, 239, 49, 111, 164, 70, 60, 2, 37, 171, 75,
    136, 156, 11, 56, 42, 146, 138, 229, 73, 146, 77, 61, 98, 196,
    135, 106, 63, 197, 195, 86, 96, 203, 113, 101, 170, 247, 181, 113,
    80, 250, 108, 7, 255, 237, 129, 226, 79, 107, 112, 166, 103, 241,
    24, 223, 239, 120, 198, 58, 60, 82, 128, 3, 184, 66, 143, 224,
    145, 224, 81, 206, 163, 45, 63, 90, 168, 114, 59, 33, 159, 95,
    28, 139, 123, 98, 125, 196, 15, 70, 194, 253, 54, 14, 109, 226,
    71, 17, 161, 93, 186, 87, 244, 138, 20, 52, 123, 251, 26, 36,
    17, 46, 52, 231, 232, 76, 31, 221, 84, 37, 216, 165, 212, 106,
    197, 242, 98, 43, 39, 175, 254, 145, 190, 84, 118, 222, 187, 136,
    120, 163, 236, 249
};

int rndindex = 0;
int prndindex = 0;

#define CASE(a) case a: return #a

//
// Converts enum value to string
//
static const char *M_prcString(pr_class pc)
{
    switch(pc)
    {
        CASE(pr_blaster);
        CASE(pr_bluespark);
        CASE(pr_boltspark);
        CASE(pr_chainwiggle);
        CASE(pr_clinkatk);
        CASE(pr_crush);
        CASE(pr_damage);
        CASE(pr_damagemobj);
        CASE(pr_dmspawn);
        CASE(pr_drawblood);
        CASE(pr_dripblood);
        CASE(pr_envirospot);
        CASE(pr_envirotics);
        CASE(pr_enviroticsend);
        CASE(pr_facetarget);
        CASE(pr_firemace);
        CASE(pr_floathealth);
        CASE(pr_gauntlets);
        CASE(pr_gauntletsangle);
        CASE(pr_ghostsneak);
        CASE(pr_goldwand);
        CASE(pr_goldwand2);
        CASE(pr_hdrop1);
        CASE(pr_hdropmom);
        CASE(pr_hereticartiteleport);
        CASE(pr_impcharge);
        CASE(pr_impcrash);
        CASE(pr_impmelee);
        CASE(pr_impmelee2);
        CASE(pr_killtics);
        CASE(pr_knightat2);
        CASE(pr_lastlook);
        CASE(pr_lichattack);
        CASE(pr_lights);
        CASE(pr_lookact);
        CASE(pr_madmelee);
        CASE(pr_makepod);
        CASE(pr_mffire);
        CASE(pr_minatk3);
        CASE(pr_mincharge);
        CASE(pr_mindist);
        CASE(pr_missrange);
        CASE(pr_moverandom);
        CASE(pr_newchase);
        CASE(pr_newchasedir);
        CASE(pr_painchance);
        CASE(pr_phoenixrod2);
        CASE(pr_plats);
        CASE(pr_podpain);
        CASE(pr_puffblood);
        CASE(pr_puffy);
        CASE(pr_ravenblood);
        CASE(pr_respawn);
        CASE(pr_rip);
        CASE(pr_ripperblood);
        CASE(pr_see);
        CASE(pr_settics);
        CASE(pr_shadow);
        CASE(pr_skullfly);
        CASE(pr_skullpop);
        CASE(pr_skullrod);
        CASE(pr_soratk2);
        CASE(pr_sorctele1);
        CASE(pr_sorctele2);
        CASE(pr_sorfx1xpl);
        CASE(pr_spawnfloat);
        CASE(pr_spawnpuff);
        CASE(pr_spawnthing);
        CASE(pr_splash);
        CASE(pr_spotspawn);
        CASE(pr_staff);
        CASE(pr_staff2);
        CASE(pr_staffangle);
        CASE(pr_svolcano);
        CASE(pr_tglit);
        CASE(pr_trywalk);
        CASE(pr_volcano);
        CASE(pr_whirlseek);
        CASE(pr_whirlwind);
        CASE(pr_wpnreadysnd);
        default:
            return "WARNING: Unexpected prclass";
    }
}

int P_Random(void)
{
    prndindex = (prndindex + 1) & 0xff;
    return rndtable[prndindex];
}

//
// Classed random
//
int P_RandomC(pr_class pc)
{
    int result = P_Random();
    printf("%s: %d: %d\n", M_prcString(pc), gametic, result);
    return result;
}

int M_Random(void)
{
    rndindex = (rndindex + 1) & 0xff;
    return rndtable[rndindex];
}

void M_ClearRandom(void)
{
    rndindex = prndindex = 0;
}

// inspired by the same routine in Eternity, thanks haleyjd
int P_SubRandom (void)
{
    int r = P_Random();
    return r - P_Random();
}

//
// Classed SubRandom
//
int P_SubRandomC(pr_class pc)
{
    int r = P_RandomC(pc);
    return r - P_RandomC(pc);
}
