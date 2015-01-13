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
//      MD5 hashing, copied from GPLv2 version of Eternity (m_hash.cpp there)
//

#include "md5.h"

#define leftrotate(w, b) (((w) << (b)) | ((w) >> (32 - (b))))

static const int md5_r[64] =
{
   7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
   5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
   4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
   6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
};

static const unsigned md5_k[64] =
{
   0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 
   0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
   0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,

   0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x2441453,
   0xd8a1e681, 0xe7d3fbc8, 0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
   0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a, 

   0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 
   0xf6bb4b60, 0xbebfbc70, 0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x4881d05,
   0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,

   0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92,
   0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
   0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

void md5_initialize(md5_HashData* hash)
{
	hash->digest[0] = 0x67452301;
	hash->digest[1] = 0xEFCDAB89;
	hash->digest[2] = 0x98BADCFE;
	hash->digest[3] = 0x10325476;
}

static void md5_processBlock(md5_HashData* hash)
{
	int i;
	unsigned temp;
	unsigned work[16];
	unsigned a, b, c, d;
	unsigned f, g;
	unsigned char* message = hash->message;
	for(i = 0; i < 16; ++i)
	{
		work[i]  = ((unsigned)message[i * 4    ]);
		work[i] |= ((unsigned)message[i * 4 + 1]) <<  8;
		work[i] |= ((unsigned)message[i * 4 + 2]) << 16;
		work[i] |= ((unsigned)message[i * 4 + 3]) << 24;
	}
	a = hash->digest[0];
	b = hash->digest[1];
	c = hash->digest[2];
	d = hash->digest[3];

	for(i = 0; i < 16; ++i)
	{
		f = (b & c) | (~b & d);
		g = i;

		temp = d;
		d = c;
		c = b;
		b = b + leftrotate((a + f + md5_k[i] + work[g]), md5_r[i]);
		a = temp;
	}

	for(; i < 32; ++i)
	{
		f = (d & b) | (~d & c);
		g = (5 * i + 1) % 16;

		temp = d;
		d = c;
		c = b;
		b = b + leftrotate((a + f + md5_k[i] + work[g]), md5_r[i]);
		a = temp;
	}

	for(; i < 48; ++i)
	{
		f = b ^ c ^ d;
		g = (3 * i + 5) % 16;

		temp = d;
		d = c;
		c = b;
		b = b + leftrotate((a + f + md5_k[i] + work[g]), md5_r[i]);
		a = temp;
	}

	for(; i < 64; ++i)
	{
		f = c ^ (b | ~d);
		g = (7 * i) % 16;

		temp = d;
		d = c;
		c = b;
		b = b + leftrotate((a + f + md5_k[i] + work[g]), md5_r[i]);
		a = temp;
	}

	hash->digest[0] += a;
	hash->digest[1] += b;
	hash->digest[2] += c;
	hash->digest[3] += d;

	hash->messageidx = 0;
}

void md5_digestData(md5_HashData* hash, const unsigned char* data, unsigned len)
{
	if(!len || hash->gonebad)
		return;

	while(len--)
	{
		hash->message[hash->messageidx++] = *data;
		hash->messagelen += 8;
		if(!hash->messagelen)
		{
			hash->gonebad = 1;
			break;
		}
		if(hash->messageidx == 64)
			md5_processBlock(hash);
		++data;
	}
}

static void md5_wrapUp(md5_HashData* hash)
{
	int i;

	if(hash->messageidx > 64 - 9)
	{
		hash->message[hash->messageidx++] = 0x80;
		while(hash->messageidx < 64)
			hash->message[hash->messageidx++] = 0;
		md5_processBlock(hash);
		while(hash->messageidx < 64)
			hash->message[hash->messageidx++] = 0;
	}
	else
	{
		hash->message[hash->messageidx++] = 0x80;
		while(hash->messageidx < 64)
			hash->message[hash->messageidx++] = 0;
	}

	hash->message[56] = (unsigned char)((hash->messagelen      ) & 0xFF);
	hash->message[57] = (unsigned char)((hash->messagelen >>  8) & 0xFF);
	hash->message[58] = (unsigned char)((hash->messagelen >> 16) & 0xFF);
	hash->message[59] = (unsigned char)((hash->messagelen >> 24) & 0xFF);

	md5_processBlock(hash);

	for(i = 0; i < 4; ++i)
	{
		unsigned word = hash->digest[i];
		hash->digest[i] = (((word << 8 ) | (word >> 24)) & 0x00ff00ff) |
						  (((word << 24) | (word >> 8 )) & 0xff00ff00);
	}
}