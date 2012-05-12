/*	list.c - doubly-linked dynamic lists
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

#include "list.h"

#include <stdlib.h>

typedef struct item_s item_t;

struct item_s {
	item_t* previous;
	item_t* next;
	void* contents;
};

struct list_s {
	item_t* first;
	item_t* last;
	item_t* cursor;
	uint size;
};

list_t* new_list()
{
	list_t* q = ALLOC(list_t);
	q->first = NULL;
	q->last = NULL;
	q->cursor = NULL;
	q->size = 0;
	return q;
}

void free_list(list_t* q)
{
	list_clear(q);
	free(q);
}

void list_clear(list_t* q)
{
	while (q->first != NULL)
		list_pop_front(q);
}

void list_free_contents(list_t* q)
{
	while (q->first != NULL)
		free(list_pop_front(q));
}

uint list_size(list_t* q)
{
	return q->size;
}

static void cursor_step_forward(list_t* q)
{
	q->cursor = (q->cursor) ? q->cursor->next : q->first;
}

static void cursor_step_backward(list_t* q)
{
	q->cursor = (q->cursor) ? q->cursor->previous : q->last;
}

list_t* list_copy(list_t* old);

list_t* list_weak_copy(list_t* old)
{
	list_t* copy = new_list();
	copy->first = old->first;
	copy->last = old->last;
	copy->cursor = old->cursor;
	copy->size = old->size;
	return copy;
}

void list_push_back(list_t* q, void* contents)
{
	item_t* item;
	if (contents == NULL)
		return;

	item = ALLOC(item_t);
	item->previous = q->last;
	item->next = NULL;
	item->contents = contents;

	if (q->last != NULL)
		q->last->next = item;
	q->last = item;
	if (q->first == NULL)
		q->first = item;
	q->size++;
}

void* list_pop_front(list_t* q)
{
	void* contents;

	item_t* item = q->first;
	if (item == NULL)
		return NULL;
	q->first = item->next;
	if (q->first != NULL)
		q->first->previous = NULL;
	q->size--;

	if (q->cursor == item) /* TODO: is this better than backward? */
		cursor_step_forward(q); 

	contents = item->contents;
	free(item);
	return contents;
}

void* list_peek_front(list_t* q)
{
	item_t* item = q->first;
	return (item) ? item->contents : NULL;
}

void* list_iterate(list_t* q)
{
	cursor_step_forward(q);
	return (q->cursor) ? q->cursor->contents : NULL;
}

void* list_retract(list_t* q)
{
	cursor_step_backward(q);
	return (q->cursor) ? q->cursor->contents : NULL;
}

void list_restart(list_t* q)
{
	q->cursor = NULL;
}

void* list_inspect(list_t* q)
{
	if (q->cursor == NULL)
		return NULL;
	return q->cursor->contents;
}

void* list_push_before(list_t* q, void* contents);

void* list_push_after(list_t* q, void* contents);

void* list_replace(list_t* q, void* contents)
{
	void* old_contents;
	item_t* item;
	if (contents == NULL)
		return list_unlink(q);
	if (q->cursor == NULL)
		return NULL;

	item = ALLOC(item_t);
	item->next = q->cursor->next;
	item->previous = q->cursor->previous;
	old_contents = q->cursor->contents;

	free(q->cursor);
	q->cursor = item;

	return old_contents;
}

void* list_unlink(list_t* q)
{
	void* contents;
	if (q->cursor == NULL)
		return NULL;
	if (q->cursor->next != NULL)
		q->cursor->next->previous = q->cursor->previous;
	if (q->cursor->previous != NULL)
		q->cursor->previous->next = q->cursor->next;
	contents = q->cursor->contents;
	free(q->cursor); /* FIXME: free next iteration */
	q->size--;
	return contents;
}

