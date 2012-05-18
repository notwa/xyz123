/*	image.h - generic 256-color image
*/

#ifndef IMAGE_H_
#define IMAGE_H_

#include <stdint.h>

typedef struct image_s {
	uint16_t width;
	uint16_t height;
	uint8_t* palette;
	uint8_t* pixels;
} image_t;

#endif

