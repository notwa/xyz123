#include "gif.h"

#include <gif_lib.h>

int gif_read(image_t* image, FILE* input)
{
	int status = GIF_NO_ERROR;
	return status;
}

int gif_write(image_t* image, FILE* output)
{
	int status = GIF_NO_ERROR;
	int fn = fileno(output);
	GifFileType* gif;
	if ((gif = EGifOpenFileHandle(fn)) == NULL
	|| EGifSpew(gif))
		status = GIF_ERROR;
	return status;
}

