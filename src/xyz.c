#include "xyz.h"
#include "easyflate.h"

#include <assert.h> /* TODO: temporary */

enum {
	HEADER_SIZE = 8
};

static uint decompress(FILE* input, uchar** output)
{
	uint filesize;
	uint length;
	uchar* compressed;
	uchar* decompressed = NULL;
	uchar* full;

	/* read header */
	full = CALLOC(uchar, HEADER_SIZE);
	fread(full, HEADER_SIZE, sizeof(uchar), input);

	/* read compressed data */
	filesize = fsize(input);
	assert(filesize > 0);
	filesize -= HEADER_SIZE;
	compressed = CALLOC(uchar, filesize);
	fread(compressed, filesize, sizeof(uchar), input);

	length = easy_inflate(compressed, filesize, &decompressed);

	/* concatenate decompressed data to "full" */
	full = REALLOC(uchar, full, length + HEADER_SIZE);
	memcpy(full + HEADER_SIZE, decompressed, length);

	free(compressed);
	free(decompressed);
	*output = full;
	return length + HEADER_SIZE;
}

static void read_header(image_t* image, uchar* data, uint length)
{
	int i;
	char id[5];

	assert(length >= HEADER_SIZE);

	for (i = 0; i < 4; i++)
		id[i] = data[i];
	id[4] = '\0';
	assert(!strcmp(id, "XYZ1"));

	image->width = data[4] + (data[5] << 8);
	image->height = data[6] + (data[7] << 8);
}

static void read_pixels(image_t* image, uchar* data, uint length)
{
	int i;
	int area = image->width * image->height;
	int palette_size = length - area;
	int pixel_size = length - palette_size;

	assert(length >= image->width * image->height);

	palette_size = length - area;
	assert(palette_size % 256 == 0);
	image->channels = palette_size / 256;

	image->palette = CALLOC(uint8_t, palette_size);
	for (i = 0; i < palette_size; i++)
		image->palette[i] = data[i];

	image->pixels = CALLOC(uint8_t, length - palette_size);
	for (i = 0; i < pixel_size; i++)
		image->pixels[i] = data[i + palette_size];
}

uint xyz_read(image_t* image, FILE* input)
{
	uchar* data = NULL;
	uint length = decompress(input, &data);
	read_header(image, data, length - HEADER_SIZE);
	read_pixels(image, data + HEADER_SIZE, length - HEADER_SIZE);
	free(data);
	return XYZ_NO_ERROR;
}

