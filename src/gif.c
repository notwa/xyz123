/* gif.h - gif image format conversion
 * Copyright (C) 2012 Connor Olding
 *
 * This program is licensed under the terms of the GNU General Public
 * License (version 3), and is distributed without any warranty.
 * You should have received a copy of the license in the file COPYING.
 * If not, visit http://gnu.org/licenses/ to obtain one.
 */

#include "gif.h"

#include <gif_lib.h>

int gif_read(image_t* image, FILE* input)
{
	int status = GIF_NO_ERROR;
	return status;
}

int gif_write(image_t* image, FILE* output)
{
	int status = GIF_NO_ERROR;
	int fn = fileno(output);
	GifFileType* gif;
	if ((gif = EGifOpenFileHandle(fn)) == NULL
	|| EGifSpew(gif))
		status = GIF_ERROR;
	return status;
}

