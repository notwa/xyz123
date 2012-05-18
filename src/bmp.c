/*	bmp.c - reading & writing of the bmp images
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

#include "bmp.h"

#include <stdint.h>
#include <assert.h>

enum {
	HEADER_SIZE = 14,
	DIB_SIZE = 40,
	FULL_SIZE = 14 + 40
};

typedef struct bmp_header_s {
	uint32_t filesize;
	uint32_t creator;
	uint32_t data_ptr;
} bmp_header_t;

typedef struct bmp_dib_header_s {
	uint32_t header_size;
	int32_t width;
	int32_t height;
	uint16_t planes;
	uint16_t bits_per_pixel;
	uint32_t compression;
	uint32_t data_size;
	int32_t horizontal_resolution;
	int32_t vertical_resolution;
	uint32_t colors;
	uint32_t important_colors;
} bmp_dib_header_t;

static void write_bmp_header(image_t* image, FILE* output)
{
	int area = image->width * image->height;
	bmp_header_t head = { FULL_SIZE, 0, FULL_SIZE };
	head.filesize += area * image->channels;
	putc('B', output);
	putc('M', output);
	fwrite(&head, sizeof(bmp_header_t), 1, output);
}

static void write_dib_header(image_t* image, FILE* output)
{
	int area = image->width * image->height;
	bmp_dib_header_t dib = {
		DIB_SIZE, 0, 0, 1, 8, 0, 0, 3779, 3779, 0, 0 };
	dib.width = image->width;
	dib.height = image->height;
	dib.bits_per_pixel *= image->channels;
	dib.data_size = area * image->channels;
	fwrite(&dib, sizeof(bmp_dib_header_t), 1, output);
}

static void write_pixel(image_t* image, FILE* output, int i)
{
	uint index = image->pixels[i] * image->channels;
	putc(image->palette[index + 2], output);
	putc(image->palette[index + 1], output);
	putc(image->palette[index + 0], output);
	if (image->channels == 4)
		putc(image->palette[index + 3], output);
}

static int read_headers(image_t* image, FILE* input)
{
	int filesize = fsize(input);
	int area, palette_size, pixel_size;
	char id[3];
	bmp_header_t head;
	bmp_dib_header_t dib;

	if (filesize < FULL_SIZE)
		return BMP_EOF_ERROR;

	fread(id, 2, 1, input);
	id[2] = '\0';
	if (strcmp(id, "BM"))
		return BMP_FORMATTING_ERROR;

	fread(&head, HEADER_SIZE - 2, 1, input);
	fread(&dib, DIB_SIZE, 1, input);

	if (dib.width >= 0x10000 || dib.height >= 0x10000)
		return BMP_UNSUPPORTED_ERROR;
	image->width = dib.width;
	image->height = dib.height;

	image->channels = dib.bits_per_pixel / 8;
	if (image->channels < 3 || image->channels > 4)
		return BMP_UNSUPPORTED_ERROR;

	area = image->width * image->height;
	palette_size = 0x100 * image->channels;
	pixel_size = area * image->channels;
	if (filesize < FULL_SIZE + pixel_size)/* + palette_size);*/
		return BMP_EOF_ERROR;

	image->palette = CALLOC(uint8_t, palette_size);
	image->pixels = CALLOC(uint8_t, area);

	return BMP_NO_ERROR;
}

static uchar find_pixel(uint32_t* palette, int *psize, uint32_t pixel)
{
	int i;
	for (i = 0; i < *psize; i++) {
		if (palette[i] == pixel)
			return i;
	}
	palette[*psize] = pixel;
	(*psize)++;
	assert(*psize < 256);
	return *psize - 1;
}

static int read_pixels(image_t* image, FILE* input)
{
	int r, g, b;
	int a = 0;
	int i;
	uint32_t palette[256];
	int psize = 0;
	int area = image->width * image->height;
	int pixel_n = area - image->width;
	while (1) {
		uint32_t pixel;
		if ((b = getc(input)) == EOF)
			break;
		else if ((g = getc(input)) == EOF)
			return BMP_FORMATTING_ERROR;
		else if ((r = getc(input)) == EOF)
			return BMP_FORMATTING_ERROR;
		else if (image->channels == 4 && (a = getc(input)) == EOF)
			return BMP_FORMATTING_ERROR;

		pixel = (r << 24) + (g << 16) + (b << 8) + a;
		image->pixels[pixel_n] = find_pixel(palette, &psize, pixel);
		pixel_n++;
		if (pixel_n % image->width == 0)
			pixel_n -= image->width * 2;
	}
	if (pixel_n != -image->width)
		return BMP_EOF_ERROR;

	for (i = 0; i < 256; i++) {
		image->palette[i * 3 + 0] = (palette[i] >> 24) & 0xFF;
		image->palette[i * 3 + 1] = (palette[i] >> 16) & 0xFF;
		image->palette[i * 3 + 2] = (palette[i] >> 8) & 0xFF;
		if (image->channels == 4)
			image->palette[i * 3 + 3] = palette[i] & 0xFF;
	}

	return BMP_NO_ERROR;
}

int bmp_read(image_t* image, FILE* input)
{
	int status = BMP_NO_ERROR;

	if ((status = read_headers(image, input)));
	else if ((status = read_pixels(image, input)));

	return status;
}

int bmp_write(image_t* image, FILE* output)
{
	int x, y;
	int status = BMP_NO_ERROR;
	write_bmp_header(image, output);
	write_dib_header(image, output);
	for (y = image->height - 1; y >= 0; y--) {
		for (x = 0; x < image->width; x++) {
			write_pixel(image, output, x + y * image->width);
		}
	}
	return status;
}

