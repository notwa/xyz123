/* gif.h - gif image format conversion
 * Copyright (C) 2012 Connor Olding
 *
 * This program is licensed under the terms of the MIT License, and
 * is distributed without any warranty.  You should have received a
 * copy of the license along with this program; see the file LICENSE.
 */

enum gif_errors {
	GIF_NO_ERROR,
	GIF_LIB_ERROR
};

/* returns non-zero on error */
int gif_read(image_t* image, FILE* input);
int gif_write(image_t* image, FILE* output);

