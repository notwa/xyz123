/* macros.c - generic functions and definitions to reduce codesize
 * Copyright (C) 2012 Connor Olding
 *
 * This program is licensed under the terms of the MIT License, and
 * is distributed without any warranty.  You should have received a
 * copy of the license along with this program; see the file LICENSE.
 */

#include "macros.h"

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

void* safe_malloc(size_t size)
{
	void* new_ptr = malloc(size);
	if (new_ptr == NULL) {
		fprintf(stderr, "Could not malloc %u bytes.\n",
			(uint) size);
		exit(1);
	}
	return new_ptr;
}

void* safe_calloc(size_t num, size_t size)
{
	void* new_ptr = calloc(num, size);
	if (new_ptr == NULL) {
		fprintf(stderr, "Could not calloc %u * %u bytes.\n",
			(uint) num, (uint) size);
		exit(1);
	}
	return new_ptr;
}

void* safe_realloc(void* ptr, size_t size)
{
	ptr = realloc(ptr, size);
	if (ptr == NULL) {
		fprintf(stderr, "Could not realloc %p with %u bytes.\n",
			ptr, (uint) size);
		exit(1);
	}
	return ptr;
}

