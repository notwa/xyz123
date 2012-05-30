/* buffs.c - simplified buffered streams
 * Copyright (C) 2012 Connor Olding
 *
 * This program is licensed under the terms of the GNU General Public
 * License (version 3), and is distributed without any warranty.
 * You should have received a copy of the license in the file COPYING.
 * If not, visit http://gnu.org/licenses/ to obtain one.
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

static void bopen(buffs_t* buffs, char* filename, char* mode, FILE* default_)
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
	bopen(buffs, filename, "rb", stdin);
}

void buffs_open_output(buffs_t* buffs, char* filename)
{
	bopen(buffs, filename, "wb", stdout);
}

void buffs_close(buffs_t* buffs)
{
	fclose(buffs->stream);
}

