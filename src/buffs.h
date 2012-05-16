/*	buffs.h - simplified buffered streams
	Copyright (C) 2012 Connor Olding

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BUFFS_H_
#define BUFFS_H_

/*  If we're using Microsoft's compiler, set the appropriate
 *  argument for freopen. If we don't do this, the program will
 *  compile but not run. Thanks, M$. */
#ifdef _MSC_VER
	#define FREOPEN_BLANK ("")
#else
	#define FREOPEN_BLANK (NULL)
#endif

#include "macros.h"

typedef struct {
	FILE* stream;
	uint size;
	char* buffer;
} buffs_t;

uint buffs_size;

void buffs_open(buffs_t* buffs, char* filename, char* mode, FILE* default_);
void buffs_close(buffs_t* buffs);

#endif

