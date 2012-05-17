/*	xyz.h - xyz image format decoding
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

#ifndef XYZ1_H_
#define XYZ1_H_

#include "macros.h"
#include "image.h"

enum xyz_errors {
	XYZ_NO_ERROR,
	XYZ_FORMATTING_ERROR,
	XYZ_ZLIB_ERROR,
	XYZ_EOF_ERROR
};

/* returns non-zero on error */
uint xyz_read(image_t* image, FILE* input);
uint xyz_write(image_t* image, FILE* output);

#endif

