/* gif.h - gif image format conversion
 * Copyright (C) 2012 Connor Olding
 *
 * This program is licensed under the terms of the GNU General Public
 * License (version 3), and is distributed without any warranty.
 * You should have received a copy of the license in the file COPYING.
 * If not, visit http://gnu.org/licenses/ to obtain one.
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

