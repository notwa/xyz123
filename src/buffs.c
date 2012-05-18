/*	buffs.c - simplified buffered streams
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

static char is_default_filename(char* s)
{
	if (s != NULL && s[0] == '-' && s[1] == '\0')
		return 1;
	return 0;
}

buffs_t* new_buffs(uint size)
{
	buffs_t* buffs = ALLOC(buffs_t);
	buffs->size = size;
	buffs->buffer = CALLOC(char, size);
	return buffs;
}

void free_buffs(buffs_t* buffs)
{
	free(buffs->buffer);
	free(buffs);
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
	setbuf(buffs->stream, buffs->buffer);
}

void buffs_open_input(buffs_t* buffs, char* filename)
{
	open(buffs, filename, "rb", stdin);
}

void buffs_open_output(buffs_t* buffs, char* filename)
{
	open(buffs, filename, "wb", stdout);
}

void buffs_close(buffs_t* buffs)
{
	fclose(buffs->stream);
}

