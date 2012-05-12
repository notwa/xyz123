/*	buffs.c - simplified buffer streams
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

#include "buffs.h"

#include <stdlib.h>

uint buffs_size = 256 * 1024;

static char is_default_filename(char* s)
{
	if (s != NULL && s[0] == '-' && s[1] == '\0')
		return 1;
	return 0;
}

static void open(buffs_t* buffs, char* filename, char* mode, FILE* default_)
{
	if (default_ != NULL && is_default_filename(filename)) {
		freopen(FREOPEN_BLANK, mode, default_);
		buffs->stream = default_;
	} else {
		buffs->stream = fopen(filename, mode);
	}
	if (buffs->stream == NULL) {
		fprintf(stderr, "Could not open file %s with mode %s.\n",
			filename, mode);
		exit(1);
	}
}

static void deallocate(buffs_t* buffs)
{
	if (buffs->size == 0)
		return;
	free(buffs->buffer);
	buffs->size = 0;
}

static void allocate(buffs_t* buffs)
{
	if (buffs->size != 0)
		deallocate(buffs);

	buffs->buffer = CALLOC(char, buffs_size);

	buffs->size = buffs_size;
	setbuf(buffs->stream, buffs->buffer);
}

void buffs_open(buffs_t* buffs, char* filename, char* mode, FILE* default_)
{
	open(buffs, filename, mode, default_);
	if (buffs_size > 0)
		allocate(buffs);
} 

void buffs_close(buffs_t* buffs)
{
	fclose(buffs->stream);
	deallocate(buffs);
}

