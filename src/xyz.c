#include "xyz.h"
#include "easyflate.h"

#include <assert.h> /* TODO: temporary */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

enum {
	XYZ_HEADER_LENGTH = 8
};

static size_t fsize(FILE* file)
{
	int position;
	int size;
	position = ftell(file);
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, position, SEEK_SET);
	return size;
}

static uint decompress(FILE* input, uchar** decompressed)
{
	uint filesize;
	uchar* compressed;
	uint length;

	filesize = fsize(input) - 8;

	compressed = CALLOC(uchar, filesize);
	fread(compressed, filesize, sizeof(uchar), input);

	length = easy_inflate(compressed, filesize, decompressed);

	free(compressed);
	return length;
}

uint xyz_read(image_t* image, FILE* input)
{
	int i;
	int area;
	int palette_size;
	int pixel_size;
	uint length;
	uchar header[XYZ_HEADER_LENGTH];
	char id[5] = "\0\0\0\0";
	uchar* decompressed = NULL;

	/* read header */
	fread(header, XYZ_HEADER_LENGTH, sizeof(uchar), input);

	for (i = 0; i < 4; i++)
		id[i] = header[i];
	assert(!strcmp(id, "XYZ1"));

	image->width = header[4] + (header[5] << 8);
	image->height = header[6] + (header[7] << 8);

	/* decompress data */
	length = decompress(input, &decompressed);
	assert(decompressed != NULL);

	/* read palette */
	area = image->width * image->height;

	palette_size = length - area;
	assert(palette_size % 256 == 0);
	image->channels = palette_size / 256;

	image->palette = CALLOC(uint8_t, palette_size);

	for (i = 0; i < palette_size; i++)
		image->palette[i] = decompressed[i];

	/* read pixels */
	pixel_size = length - palette_size;

	image->pixels = CALLOC(uint8_t, pixel_size);

	for (i = 0; i < pixel_size; i++)
		image->pixels[i] = decompressed[i + palette_size];

	/* finish */
	free(decompressed);
	return XYZ_NO_ERROR;
}

