#include <stdio.h>
#include "futil.h"

size_t fsize(FILE* file)
{
	int position;
	int size;
	position = ftell(file);
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, position, SEEK_SET);
	return size;
}
