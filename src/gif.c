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

enum {
	GCE_ID = 0xF9,
	GCE_LENGTH = 4
};

static char gce_data[GCE_LENGTH] = {
	0x01, /* there is a transparent color */
	0x00, /* unused */
	0x00, /* color #0 is transparent */
	0x00 /* end of extension */
};

int gif_read(image_t* image, FILE* input)
{
	int status = GIF_NO_ERROR;

	/* image->palette = CALLOC(uint8_t, 256 * 3); */

	return status;
}

int open_handler(FILE* output, GifFileType** gif)
{
	int fn = fileno(output);
	if ((*gif = EGifOpenFileHandle(fn)) == NULL)
		return GIF_LIB_ERROR;
	return GIF_NO_ERROR;
}

int make_palette(image_t* image, ColorMapObject** gif_palette)
{
	int i;

	GifColorType colors[256];
	
	for (i = 0; i < 256; i++) {
		colors[i].Red = image->palette[i * 3 + 0];
		colors[i].Green = image->palette[i * 3 + 1];
		colors[i].Blue = image->palette[i * 3 + 2];
	}
	*gif_palette = MakeMapObject(256, colors);

	return GIF_NO_ERROR;
}

int write_data(image_t* image, GifFileType* gif, ColorMapObject* gif_palette)
{
	int i;

	if (EGifPutScreenDesc(gif, image->width, image->height,
	256, 0, gif_palette) == GIF_ERROR)
		return GIF_LIB_ERROR;

	if (EGifPutExtension(gif, GCE_ID, GCE_LENGTH, gce_data) == GIF_ERROR)
		return GIF_LIB_ERROR;

	if (EGifPutImageDesc(gif, 0, 0,
	image->width, image->height, FALSE, NULL) == GIF_ERROR)
		return GIF_LIB_ERROR;

	for (i = 0; i < image->height; i++) {
		if (EGifPutLine(gif,
		image->pixels + i * image->width, image->width) == GIF_ERROR)
			return GIF_LIB_ERROR;
	}

	if (EGifSpew(gif))
		return GIF_LIB_ERROR;

	return GIF_NO_ERROR;
}

int gif_write(image_t* image, FILE* output)
{
	int status = GIF_NO_ERROR;
	GifFileType* gif = NULL;
	ColorMapObject* gif_palette = NULL;

	if ((status = open_handler(output, &gif)));
	else if ((status = make_palette(image, &gif_palette)));
	else if ((status = write_data(image, gif, gif_palette)));

	if (gif)
		EGifCloseFile(gif);
	if (gif_palette)
		FreeMapObject(gif_palette);
	return status;
}

