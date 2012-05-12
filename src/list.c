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
	list_t* list = ALLOC(list_t);
	list->first = NULL;
	list->last = NULL;
	list->cursor = NULL;
	list->size = 0;
	return list;
}

void free_list(list_t* list)
{
	list_clear(list);
	free(list);
}

void list_clear(list_t* list)
{
	while (list->first != NULL)
		list_pop_front(list);
}

void list_free_contents(list_t* list)
{
	while (list->first != NULL)
		free(list_pop_front(list));
}

uint list_size(list_t* list)
{
	return list->size;
}

list_t* list_copy(list_t* list)
{
	list_t* copy = new_list();
	copy->size = list->size;

	item_t* old = list->first;
	item_t* new;
	item_t* previous = NULL;
	while (old != NULL) {
		new = ALLOC(item_t);
		new->previous = previous;
		new->next = NULL;
		new->contents = old->contents;

		if (previous != NULL)
			previous->next = new;
		else
			copy->first = new;

		if (list->cursor == old)
			copy->cursor = new;

		previous = new;
		old = old->next;
	}
	copy->last = new;

	return copy;
}

list_t* list_weak_copy(list_t* old)
{
	list_t* copy = new_list();
	copy->first = old->first;
	copy->last = old->last;
	copy->cursor = old->cursor;
	copy->size = old->size;
	return copy;
}

void list_reverse(list_t* list)
{
	item_t* item = list->first;
	item_t* next;
	while (item != NULL) {
		next = item->next;
		item->next = item->previous;
		item->previous = next;
		item = next;
	}
	next = list->last;
	list->last = list->first;
	list->first = next;
}

static void cursor_step_forward(list_t* list)
{
	list->cursor = (list->cursor) ? list->cursor->next : list->first;
}

static void cursor_step_backward(list_t* list)
{
	list->cursor = (list->cursor) ? list->cursor->previous : list->last;
}

void list_push_front(list_t* list, void* contents)
{
	item_t* item;
	if (contents == NULL)
		return;

	item = ALLOC(item_t);
	item->next = list->first;
	item->previous = NULL;
	item->contents = contents;

	if (list->first != NULL)
		list->first->previous = item;
	list->first = item;
	if (list->last == NULL)
		list->last = item;
	list->size++;
}

void list_push_back(list_t* list, void* contents)
{
	item_t* item;
	if (contents == NULL)
		return;

	item = ALLOC(item_t);
	item->previous = list->last;
	item->next = NULL;
	item->contents = contents;

	if (list->last != NULL)
		list->last->next = item;
	list->last = item;
	if (list->first == NULL)
		list->first = item;
	list->size++;
}

void* list_pop_front(list_t* list)
{
	void* contents;

	item_t* item = list->first;
	if (item == NULL)
		return NULL;
	list->first = item->next;
	if (list->first != NULL)
		list->first->previous = NULL;
	list->size--;

	if (list->cursor == item)
		cursor_step_forward(list); 

	contents = item->contents;
	free(item);
	return contents;
}

void* list_pop_back(list_t* list)
{
	void* contents;

	item_t* item = list->last;
	if (item == NULL)
		return NULL;
	list->last = item->previous;
	if (list->last != NULL)
		list->last->next = NULL;
	list->size--;

	if (list->cursor == item)
		cursor_step_backward(list); 

	contents = item->contents;
	free(item);
	return contents;
}

void* list_peek_front(list_t* list)
{
	item_t* item = list->first;
	return (item) ? item->contents : NULL;
}

void* list_peek_back(list_t* list)
{
	item_t* item = list->last;
	return (item) ? item->contents : NULL;
}

void* list_iterate(list_t* list)
{
	cursor_step_forward(list);
	return (list->cursor) ? list->cursor->contents : NULL;
}

void* list_retract(list_t* list)
{
	cursor_step_backward(list);
	return (list->cursor) ? list->cursor->contents : NULL;
}

void list_restart(list_t* list)
{
	list->cursor = NULL;
}

void* list_inspect(list_t* list)
{
	if (list->cursor == NULL)
		return NULL;
	return list->cursor->contents;
}

void list_push_before(list_t* list, void* contents)
{
	item_t* item;
	item_t* previous_item;
	if (contents == NULL)
		return;
	if (list->cursor == NULL)
		return list_push_back(list, contents);
	previous_item = list->cursor->next;
	if (previous_item == NULL)
		return list_push_front(list, contents);

	item = ALLOC(item_t);
	item->next = list->cursor;
	item->previous = previous_item->previous;
	item->contents = contents;

	list->size++;
}

void list_push_after(list_t* list, void* contents)
{
	item_t* item;
	item_t* next_item;
	if (contents == NULL)
		return;
	if (list->cursor == NULL)
		return list_push_front(list, contents);
	next_item = list->cursor->next;
	if (next_item == NULL)
		return list_push_back(list, contents);

	item = ALLOC(item_t);
	item->previous = list->cursor;
	item->next = next_item->next;
	item->contents = contents;

	list->size++;
}

void* list_replace(list_t* list, void* contents)
{
	void* old_contents;
	item_t* item;
	if (contents == NULL)
		return list_unlink(list);
	if (list->cursor == NULL)
		return NULL;

	item = ALLOC(item_t);
	item->next = list->cursor->next;
	item->previous = list->cursor->previous;
	old_contents = list->cursor->contents;

	free(list->cursor);
	list->cursor = item;

	return old_contents;
}

void* list_unlink(list_t* list)
{
	void* contents;
	if (list->cursor == NULL)
		return NULL;
	if (list->cursor->next != NULL)
		list->cursor->next->previous = list->cursor->previous;
	if (list->cursor->previous != NULL)
		list->cursor->previous->next = list->cursor->next;
	contents = list->cursor->contents;
	free(list->cursor); /* FIXME: free next iteration */
	list->size--;
	return contents;
}

