/* image.h - generic 256-color image */

enum image_flag {
	IMAGE_TRANSPARENT = 0x01
};

typedef struct image_s {
	uint16_t width;
	uint16_t height;

	/* always 256 * 3 in size */
	uint8_t* palette;

	/* width * height in size */
	uint8_t* pixels;

	/* bitmask. mostly unused. see image_flag for values */
	uint8_t flags;

	/* which palette color to replace with transparency */
	uint8_t trans_key;
} image_t;

