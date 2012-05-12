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
#include <stdio.h>

enum {
	HEADER_SIZE = 14,
	DIB_SIZE = 40,
	FULL_SIZE = 14 + 40
};

typedef struct bmp_header_s
{
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

uint bmp_write(image_t* image, FILE* output)
{
	int x, y;

	write_bmp_header(image, output);
	write_dib_header(image, output);

	for (y = image->height - 1; y >= 0; y--) {
		for (x = 0; x < image->width; x++) {
			write_pixel(image, output, x + y * image->width);
		}
	}

	return 0;
}

