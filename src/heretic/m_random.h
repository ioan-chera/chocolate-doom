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

#ifndef HERETIC_M_RANDOM_H
#define HERETIC_M_RANDOM_H

// Most damage defined using HITDICE
#define HITDICE(a) ((1+(P_Random()&7))*a)

//
// Randomness class
//
typedef enum pr_class
{
    pr_blaster,
    pr_bluespark,
    pr_boltspark,
    pr_chainwiggle,
    pr_clinkatk,
    pr_crush,
    pr_damage,
    pr_damagemobj,
    pr_dmspawn,
    pr_drawblood,
    pr_dripblood,
    pr_envirospot,
    pr_envirotics,
    pr_facetarget,
    pr_firemace,
    pr_floathealth,
    pr_gauntlets,
    pr_gauntletsangle,
    pr_ghostsneak,
    pr_goldwand,
    pr_goldwand2,
    pr_hdrop1,
    pr_hdropmom,
    pr_hereticartiteleport,
    pr_impcharge,
    pr_impcrash,
    pr_impmelee,
    pr_impmelee2,
    pr_killtics,
    pr_knightat2,
    pr_lastlook,
    pr_lichattack,
    pr_lights,
    pr_lookact,
    pr_madmelee,
    pr_makepod,
    pr_mffire,
    pr_minatk3,
    pr_mincharge,
    pr_mindist,
    pr_missrange,
    pr_moverandom,
    pr_newchase,
    pr_newchasedir,
    pr_painchance,
    pr_phoenixrod2,
    pr_plats,
    pr_podpain,
    pr_puffblood,
    pr_puffy,
    pr_ravenblood,
    pr_respawn,
    pr_rip,
    pr_ripperblood,
    pr_see,
    pr_settics,
    pr_shadow,
    pr_skullfly,
    pr_skullpop,
    pr_skullrod,
    pr_soratk2,
    pr_sorctele1,
    pr_sorctele2,
    pr_sorfx1xpl,
    pr_spawnfloat,
    pr_spawnpuff,
    pr_spawnthing,
    pr_splash,
    pr_spotspawn,
    pr_staff,
    pr_staff2,
    pr_staffangle,
    pr_svolcano,
    pr_tglit,
    pr_trywalk,
    pr_volcano,
    pr_whirlseek,
    pr_whirlwind,
    pr_wpnreadysnd,
} pr_class;

int M_Random(void);
// returns a number from 0 to 255
int P_Random(void);
int P_RandomC(pr_class pc);
// as M_Random, but used only by the play simulation

void M_ClearRandom(void);
// fix randoms for demos

extern int rndindex;

// Defined version of P_Random() - P_Random()
int P_SubRandom (void);
int P_SubRandomC(pr_class pc);

#endif // HERETIC_M_RANDOM_H

