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

typedef struct buffs_s {
	FILE* stream;
	uint size;
	char* buffer;
} buffs_t;

buffs_t* new_buffs(uint size);
void free_buffs(buffs_t* buffs);

void buffs_open_input(buffs_t* buffs, char* filename);
void buffs_open_output(buffs_t* buffs, char* filename);

void buffs_close(buffs_t* buffs);

#endif

