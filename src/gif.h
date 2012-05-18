/*	gif.h - gif image format conversion
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

#ifndef GIF_H_
#define GIF_H_

#include "macros.h"
#include "image.h"

enum gif_errors {
	GIF_NO_ERROR,
	GIF_ERROR
};

/* returns non-zero on error */
int gif_read(image_t* image, FILE* input);
int gif_write(image_t* image, FILE* output);

#endif

