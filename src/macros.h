/* macros.h - generic functions and definitions to reduce codesize
 * Copyright (C) 2012 Connor Olding
 *
 * This program is licensed under the terms of the GNU General Public
 * License (version 3), and is distributed without any warranty.
 * You should have received a copy of the license in the file COPYING.
 * If not, visit http://gnu.org/licenses/ to obtain one.
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

