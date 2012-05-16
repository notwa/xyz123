#include "xyz.h"
#include "ezflate.h"

enum {
	HEADER_SIZE = 8
};

static uint decompress(FILE* input, uchar** output, uint* length)
{
	uint filesize;
	uchar* compressed;
	uchar* decompressed = NULL;
	uchar* full;

	/* read header */
	full = CALLOC(uchar, HEADER_SIZE);
	fread(full, HEADER_SIZE, sizeof(uchar), input);

	/* read compressed data */
	filesize = fsize(input);
	if (filesize <= 8)
		return XYZ_EOF_ERROR;
	filesize -= HEADER_SIZE;
	compressed = CALLOC(uchar, filesize);
	fread(compressed, filesize, sizeof(uchar), input);

	if (ez_inflate(compressed, filesize, &decompressed, length))
		return XYZ_ZLIB_ERROR;

	/* concatenate decompressed data to "full" */
	full = REALLOC(uchar, full, *length + HEADER_SIZE);
	memcpy(full + HEADER_SIZE, decompressed, *length);

	free(compressed);
	free(decompressed);
	*output = full;
	return XYZ_NO_ERROR;
}

static uint read_header(image_t* image, uchar* data, uint length)
{
	int i;
	char id[5];

	if (length < HEADER_SIZE)
		return XYZ_EOF_ERROR;

	for (i = 0; i < 4; i++)
		id[i] = data[i];
	id[4] = '\0';

	if (strcmp(id, "XYZ1"))
		return XYZ_FORMATTING_ERROR;

	image->width = data[4] + (data[5] << 8);
	image->height = data[6] + (data[7] << 8);
	return XYZ_NO_ERROR;
}

static uint read_pixels(image_t* image, uchar* data, uint length)
{
	int i;
	int area = image->width * image->height;
	int palette_size = length - area;
	int pixel_size = length - palette_size;

	if (length < area)
		return XYZ_EOF_ERROR;

	palette_size = length - area;
	if (palette_size % 256)
		return XYZ_FORMATTING_ERROR;
	image->channels = palette_size / 256;

	image->palette = CALLOC(uint8_t, palette_size);
	for (i = 0; i < palette_size; i++)
		image->palette[i] = data[i];

	image->pixels = CALLOC(uint8_t, length - palette_size);
	for (i = 0; i < pixel_size; i++)
		image->pixels[i] = data[i + palette_size];
	return XYZ_NO_ERROR;
}

uint xyz_read(image_t* image, FILE* input)
{
	uchar* data = NULL;
	uint length;
	int status = XYZ_NO_ERROR;
	if (!status)
		status = decompress(input, &data, &length);
	if (!status)
		status = read_header(image, data, length);
	if (!status)
		status = read_pixels(image, data + HEADER_SIZE, length);
	if (data)
		free(data);
	return status;
}

