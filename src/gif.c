/* gif.c - gif image format conversion
 * Copyright (C) 2012 Connor Olding
 *
 * This program is licensed under the terms of the MIT License, and
 * is distributed without any warranty.  You should have received a
 * copy of the license along with this program; see the file LICENSE.
 */

/* This is loosely based on code by Hari Nair. Specifically,
 * <http://cpansearch.perl.org/src/DHUNT/PDL-Planet-0.12/libimage/gif.c>
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "gif_lib.h"

#include "unsigned.h"
#include "image.h"
#include "gif.h"
#include "util.h"

enum {
	GCE_ID = 0xF9,
	GCE_LENGTH = 4
};

static char gce_data[GCE_LENGTH] = {
	0x01, /* there is a transparent color */
	0x00, /* unimportant */
	0x00, /* this color is trasparent (changed as needed) */
	0x00 /* end of extension */
};

static int open_dhandler(FILE* file, GifFileType** gif)
{
	int fn = fileno(file);
	if ((*gif = DGifOpenFileHandle(fn)) == NULL)
		return GIF_LIB_ERROR;
	return GIF_NO_ERROR;
}

static int read_desc(image_t* image, GifFileType* gif)
{
	int i;
	int area;
	GifPixelType* line;

	if (DGifGetImageDesc(gif) == GIF_ERROR)
		return GIF_LIB_ERROR;

	image->width = gif->Image.Width;
	image->height = gif->Image.Height;
	area = image->width * image->height;

	image->pixels = callocs(area);

	if (gif->Image.Interlace) /* TODO */
		return GIF_LIB_ERROR;

	line = (GifPixelType*) image->pixels;
	for (i = 0; i < image->height; i++) {
		if (DGifGetLine(gif, line, image->width) == GIF_ERROR)
			break;
		line += image->width;
	}

	if (i != image->height) {
		free(image->pixels);
		return GIF_LIB_ERROR;
	} else {
		return GIF_NO_ERROR;
	}
}

static int read_ext(image_t* image, int code, GifByteType* ext)
{
	/* currently we only support one extension type */
	if (code != GCE_ID)
		return GIF_NO_ERROR;
	if (ext[0] == 1) {
		image->flags |= IMAGE_TRANSPARENT;
		image->trans_key = ext[2];
	}
	return GIF_NO_ERROR;
}

static int read_ext_block(image_t* image, GifFileType* gif)
{
	int code;
	GifByteType* ext;

	if (DGifGetExtension(gif, &code, &ext) == GIF_ERROR)
		return GIF_LIB_ERROR;
	read_ext(image, code, ext);

	while (ext != NULL) {
		if (DGifGetExtensionNext(gif, &ext) == GIF_ERROR)
			return GIF_LIB_ERROR;
		if (ext != NULL)
			read_ext(image, code, ext);
	}

	return GIF_NO_ERROR;
}

static void convert_gif_palette(image_t* image, ColorMapObject* gif_palette)
{
	int i;

	image->palette = callocs(256 * 3);

	for (i = 0; i < gif_palette->ColorCount; i++) {
		GifColorType* color = &gif_palette->Colors[i];
		image->palette[i * 3 + 0] = color->Red;
		image->palette[i * 3 + 1] = color->Green;
		image->palette[i * 3 + 2] = color->Blue;
	}
}

static int read_records(image_t* image, GifFileType* gif)
{
	int status = GIF_NO_ERROR;
	GifRecordType rtype;
	while (1) {
		DGifGetRecordType(gif, &rtype);
		if (rtype == IMAGE_DESC_RECORD_TYPE)
			status = read_desc(image, gif);
		else if (rtype == EXTENSION_RECORD_TYPE)
			status = read_ext_block(image, gif);
		else if (rtype == TERMINATE_RECORD_TYPE)
			break;
		else
			status = GIF_LIB_ERROR;

		if (status == GIF_LIB_ERROR)
			break;

	}

	/* this might not be the right place to be reading this */
	if (gif->Image.ColorMap != NULL)
		convert_gif_palette(image, gif->Image.ColorMap);
	else
		convert_gif_palette(image, gif->SColorMap);

	return status;
}

int gif_read(image_t* image, FILE* input)
{
	int status = GIF_NO_ERROR;
	GifFileType* gif = NULL;
	image->flags = 0;

	if ((status = open_dhandler(input, &gif)));
	else if ((status = read_records(image, gif)));

	if (gif)
		DGifCloseFile(gif);

	return status;
}

static int open_ehandler(FILE* file, GifFileType** gif)
{
	int fn = fileno(file);
	if ((*gif = EGifOpenFileHandle(fn)) == NULL)
		return GIF_LIB_ERROR;
	return GIF_NO_ERROR;
}

static int make_palette(image_t* image, ColorMapObject** gif_palette)
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

static int write_data(image_t* image, GifFileType* gif,
		ColorMapObject* gif_palette)
{
	int i;

	if (EGifPutScreenDesc(gif, image->width, image->height,
	256, 0, gif_palette) == GIF_ERROR)
		return GIF_LIB_ERROR;

	if (image->flags && IMAGE_TRANSPARENT) {
		gce_data[2] = image->trans_key;
		if (EGifPutExtension(gif,
		GCE_ID, GCE_LENGTH, gce_data) == GIF_ERROR)
			return GIF_LIB_ERROR;
	}

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

	if ((status = open_ehandler(output, &gif)));
	else if ((status = make_palette(image, &gif_palette)));
	else if ((status = write_data(image, gif, gif_palette)));

	if (gif)
		EGifCloseFile(gif);
	if (gif_palette)
		FreeMapObject(gif_palette);
	return status;
}

