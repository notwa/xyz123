/*	easyflate.h - simplified zlib deflation/inflation
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

#ifndef EASYFLATE_H_
#define EASYFLATE_H_

#include "macros.h"

enum ez_errors {
	EZ_NO_ERROR,
	EZ_ERRNO,
	EZ_STREAM_ERROR,
	EZ_DATA_ERROR,
	EZ_MEMORY_ERROR,
	EZ_BUFFER_ERROR,
	EZ_VERSION_ERROR
};

/* returns non-zero on error */
int ez_inflate(uchar* input, uint input_length,
		uchar** output, uint* output_length);
int ez_deflate(uchar* input, uint input_length,
		uchar** output, uint* output_length);

#endif

