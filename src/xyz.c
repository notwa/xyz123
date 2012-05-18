#include "xyz.h"
#include "ezflate.h"

enum {
	HEADER_SIZE = 8
};

static int decompress(FILE* input, uchar** output, uint* length)
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

static int read_header(image_t* image, uchar* data, uint length)
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

static int read_pixels(image_t* image, uchar* data, uint length)
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

int xyz_read(image_t* image, FILE* input)
{
	uchar* data = NULL;
	uint length;
	int status = XYZ_NO_ERROR;
	if ((status = decompress(input, &data, &length)));
	else if ((status = read_header(image, data, length)));
	else if ((status = read_pixels(image, data + HEADER_SIZE, length)));
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

static int write_data(image_t* image, FILE* output)
{
	int status;
	uint length;
	int palette_size = 256 * image->channels;
	int pixel_size = image->width * image->height;
	int total_size = palette_size + pixel_size;

	uchar* data = CALLOC(uchar, total_size);
	uchar* compressed = CALLOC(uchar, total_size);

	memcpy(data, image->palette, palette_size);
	memcpy(data + palette_size, image->pixels, pixel_size);
	status = ez_deflate(data, total_size, &compressed, &length);

	if (!status)
		fwrite(compressed, length, 1, output);

	free(data);
	free(compressed);
	return XYZ_NO_ERROR;
}

int xyz_write(image_t* image, FILE* output)
{
	int status = XYZ_NO_ERROR;
	if ((status = write_header(image, output)));
	else if ((status = write_data(image, output)));
	return status;
}

