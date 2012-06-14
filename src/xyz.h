/* xyz.h - xyz image format conversion
 * Copyright (C) 2012 Connor Olding
 *
 * This program is licensed under the terms of the GNU General Public
 * License (version 3), and is distributed without any warranty.
 * You should have received a copy of the license in the file COPYING.
 * If not, visit <http://gnu.org/licenses/> to obtain one.
 */

enum xyz_errors {
	XYZ_NO_ERROR,
	XYZ_FORMATTING_ERROR,
	XYZ_ZLIB_ERROR,
	XYZ_EOF_ERROR
};

/* returns non-zero on error */
int xyz_read(image_t* image, FILE* input);
int xyz_write(image_t* image, FILE* output);

