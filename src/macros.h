/*	macros.h - generic functions and definitions to reduce codesize
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

#ifndef MACROS_H_
#define MACROS_H_

#define _POSIX_SOURCE /* for fileno */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

size_t fsize(FILE* file);

void* safe_malloc(size_t size);
void* safe_calloc(size_t num, size_t size);
void* safe_realloc(void* ptr, size_t size);

#define ALLOC(type) (\
(type*) safe_malloc(sizeof(type))\
)

#define CALLOC(type, count) (\
(type*) safe_calloc(sizeof(type), (count))\
)

#define REALLOC(type, ptr, count) (\
(type*) safe_realloc((ptr), sizeof(type) * (count))\
)

#endif

