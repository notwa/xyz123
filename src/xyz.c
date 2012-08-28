/* xyz.c - xyz image format conversion
 * Copyright (C) 2012 Connor Olding
 *
 * This program is licensed under the terms of the MIT License, and
 * is distributed without any warranty.  You should have received a
 * copy of the license along with this program; see the file LICENSE.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "zlib.h"

#include "unsigned.h"
#include "futil.h"
#include "image.h"
#include "xyz.h"
#include "util.h"

enum {
	HEADER_SIZE = 8,
	PALETTE_SIZE = 256 * 3
};

static int decompress(FILE* input, uchar** data, ulong* length)
{
	int status;
	ulong filesize = fsize(input) - HEADER_SIZE;
	uchar* compressed = callocs(filesize);
	uchar* decompressed = callocs(*length);

	fread(compressed, filesize, sizeof(uchar), input);
	status = uncompress((Bytef*) decompressed, length,
		(Bytef*) compressed, filesize);

	free(compressed);
	*data = decompressed;
	return (status) ? XYZ_ZLIB_ERROR : XYZ_NO_ERROR;
}

static int read_header(image_t* image, FILE* input, ulong *length)
{
	char id[5];

	fread(id, sizeof(char), 4, input);
	if (feof(input))
		return XYZ_EOF_ERROR;

	id[4] = '\0';
	if (strcmp(id, "XYZ1"))
		return XYZ_FORMATTING_ERROR;

	image->width = getc(input) + (getc(input) << 8);
	image->height = getc(input) + (getc(input) << 8);
	*length = PALETTE_SIZE + image->width * image->height;
	return XYZ_NO_ERROR;
}

static int read_pixels(image_t* image, uchar* data, ulong length)
{
	int i;
	int area = image->width * image->height;
	int palette_size = length - area;
	int pixel_size = length - palette_size;

	if (length < area)
		return XYZ_EOF_ERROR;

	palette_size = length - area;
	if (palette_size % 256 || palette_size / 256 != 3)
		return XYZ_FORMATTING_ERROR;

	image->palette = callocs(palette_size);
	for (i = 0; i < palette_size; i++)
		image->palette[i] = data[i];

	image->pixels = callocs(length - palette_size);
	for (i = 0; i < pixel_size; i++)
		image->pixels[i] = data[i + palette_size];

	image->flags |= IMAGE_TRANSPARENT;
	image->trans_key = 0;

	return XYZ_NO_ERROR;
}

int xyz_read(image_t* image, FILE* input)
{
	uchar* data = NULL;
	ulong length = 0;
	int status = XYZ_NO_ERROR;
	if ((status = read_header(image, input, &length)));
	else if ((status = decompress(input, &data, &length)));
	else if ((status = read_pixels(image, data, length)));
	if (data)
		free(data);
	return status;
}

static int write_header(image_t* image, FILE* output)
{
	fputs("XYZ1", output);
	putc(image->width & 0xFF, output);
	putc(image->width >> 8, output);
	putc(image->height & 0xFF, output);
	putc(image->height >> 8, output);
	return XYZ_NO_ERROR;
}

/* I know this is gross and in the wrong place - FIXME later. */
static void reorder_palette(image_t* image)
{
	int i;
	int area = image->width * image->height;
	uint8_t temp[3];
	temp[0] = image->palette[0];
	temp[1] = image->palette[1];
	temp[2] = image->palette[2];
	image->palette[0] = image->palette[image->trans_key * 3 + 0];
	image->palette[1] = image->palette[image->trans_key * 3 + 1];
	image->palette[2] = image->palette[image->trans_key * 3 + 2];
	image->palette[image->trans_key * 3 + 0] = temp[0];
	image->palette[image->trans_key * 3 + 1] = temp[1];
	image->palette[image->trans_key * 3 + 2] = temp[2];

	for (i = 0; i < area; i++) {
		uint8_t* pixel = &image->pixels[i];
		if (*pixel == 0)
			*pixel = image->trans_key;
		else if (*pixel == image->trans_key)
			*pixel = 0;
	}

	image->trans_key = 0;
}

static int write_data(image_t* image, FILE* output)
{
	int status;
	int pixel_size = image->width * image->height;
	int total_size = PALETTE_SIZE + pixel_size;
	ulong length = PALETTE_SIZE + pixel_size;

	uchar* data = callocs(total_size);
	uchar* compressed = callocs(length);

	if (image->flags & IMAGE_TRANSPARENT)
		reorder_palette(image);

	memcpy(data, image->palette, PALETTE_SIZE);
	memcpy(data + PALETTE_SIZE, image->pixels, pixel_size);
	status = compress((Bytef*) compressed, &length,
		(Bytef*) data, total_size);

	if (!status)
		fwrite(compressed, length, 1, output);

	free(data);
	free(compressed);
	return (status) ? XYZ_ZLIB_ERROR : XYZ_NO_ERROR;
}

int xyz_write(image_t* image, FILE* output)
{
	int status = XYZ_NO_ERROR;
	if ((status = write_header(image, output)));
	else if ((status = write_data(image, output)));
	return status;
}

