/*	macros.c - generic functions and definitions to reduce codesize
	Copyright (C) 2012 Connor Olding

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

