/*	easyflate.c - simplified zlib deflation/inflation
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

#include "easyflate.h"

#include <zlib.h>

#include <assert.h> /* temporary */
#include <stdlib.h>

enum {
	EASY_CHUNK_SIZE = 4096
};

static z_stream* new_z_stream()
{
	z_stream *z = ALLOC(z_stream);
	z->zalloc = Z_NULL;
	z->zfree = Z_NULL;
	z->opaque = Z_NULL;
	z->next_in = Z_NULL;
	z->avail_in = 0;
	return z;
}

uint easy_inflate(uchar* input, uint input_length,
uchar** output)
{
	z_stream* z;
	int status;
	uint output_length = 0;

	z = new_z_stream();
	assert(inflateInit(z) == Z_OK);

	z->next_in = (Bytef*) input;
	z->avail_in = input_length;

	*output = CALLOC(uchar, EASY_CHUNK_SIZE);
	z->next_out = (Bytef*) *output;
	z->avail_out = EASY_CHUNK_SIZE;

	status = Z_OK;
	while (status != Z_STREAM_END) {
		if (z->avail_out <= 0) {
			int size;
			z->avail_out = EASY_CHUNK_SIZE;
			size = z->total_out + z->avail_out;
			*output = REALLOC(uchar, *output, size);
			z->next_out = (Bytef*) *output + z->total_out;
		}
		status = inflate(z, Z_NO_FLUSH);
		assert(status >= 0);
	}

	output_length = z->total_out;
	free(z);
	return output_length;
}

