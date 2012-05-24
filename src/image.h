/* image.h - generic 256-color image */

#ifndef IMAGE_H_
#define IMAGE_H_

#include <stdint.h>

typedef struct image_s {
	uint16_t width;
	uint16_t height;

	/* always 256 * 3 in size */
	uint8_t* palette;

	/* width * height in size */
	uint8_t* pixels;
} image_t;

#endif

