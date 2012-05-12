/*	list.h - doubly-linked dynamic lists
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

#ifndef LIST_H_
#define LIST_H_

#include "macros.h"

typedef struct list_s list_t;

/* General Methods */

/** Allocates and returns a new list object. */
list_t* new_list();

/** Frees the list object.
 *  Doesn't free its contents; use list_free_contents beforehand.
 *  Use this instead of free(my_list) to avoid memory leaks. */
void free_list(list_t*);

/** Empties the list.
 *  Doesn't free its contents; use list_free_contents instead. */
void list_clear(list_t*);

/** Frees and removes everything in the list.
 *  Doesn't free the list object itself. */
void list_free_contents(list_t*);

/** Returns the total items in the list. */
uint list_size(list_t*);

/** Copies the list and its items. Returns the new list. */
list_t* list_copy(list_t*);

/** Copies the list, but keeps its item references. Returns the new list. */
list_t* list_weak_copy(list_t*);

/** Reverses the list. */
void list_reverse(list_t*);

/* Stack-like Methods */

/** Appends the pointer to the start. */
void list_push_front(list_t*, void*);

/** Appends the pointer to the end. */
void list_push_back(list_t*, void*);

/** Returns and removes the first pointer. */
void* list_pop_front(list_t*);

/** Returns and removes the last pointer. */
void* list_pop_back(list_t*);

/** Returns the first pointer without removing it. */
void* list_peek_front(list_t*);

/** Returns the last pointer without removing it. */
void* list_peek_back(list_t*);

/* Iteration Methods */

/** Moves the cursor forwards and returns the pointer under it.
 *  Returns NULL at the end of the list and restarts. */
void* list_iterate(list_t*);

/** Moves the cursor backwards and returns the pointer under it.
 *  Returns NULL at the start of the list and restarts. */
void* list_retract(list_t*);

/** Moves the cursor back to its default position. */
void list_restart(list_t*);

/** Returns the pointer under the cursor. */
void* list_inspect(list_t*);

/** Adds the pointer before the cursor.
 *  Note that calling retract afterwards will return this pointer. */
void* list_push_before(list_t*, void*);

/** Adds the pointer after the cursor.
 *  Note that calling iterate afterwards will return this pointer. */
void* list_push_after(list_t*, void*);

/** Returns and replaces the current pointer. */
void* list_replace(list_t*, void*);

/** Returns and removes the current pointer. The cursor doesn't move. */
void* list_unlink(list_t*);

#endif

