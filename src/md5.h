//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2009 James Haley
// Copyright(C) 2015 Ioan Chera
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
// DESCRIPTION:
//      MD5 hashing, copied from GPLv2 version of Eternity
//    

#ifndef MD5_H_
#define MD5_H_

typedef struct md5_HashData
{
	unsigned digest[4];
	unsigned char message[64];
	int messageidx;
	unsigned messagelen;
	unsigned gonebad:1;
} md5_HashData;

void md5_initialize(md5_HashData* hash);
void md5_digestData(md5_HashData* hash, const unsigned char* data, unsigned len);
void md5_wrapUp(md5_HashData* hash);
char* md5_digestToString(const md5_HashData* hash);

#endif
