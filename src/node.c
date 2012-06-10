/* node.c - one-dimensional linked nodes to act as linked lists
 * Copyright (C) 2012 Connor Olding
 *
 * This program is licensed under the terms of the GNU General Public
 * License (version 3), and is distributed without any warranty.
 * You should have received a copy of the license in the file COPYING.
 * If not, visit http://gnu.org/licenses/ to obtain one.
 */

#include "node.h"

node_t* node_new_null()
{
	return node_new(NULL, NULL, NULL);
}

node_t* node_new(node_t* prev, node_t* next, void* contents)
{
	node_t* node = ALLOC(node_t);
	node->prev = prev;
	node->next = next;
	node->contents = contents;
	return node;
}

void free_node(node_t* node)
{
	free(node);
}

void node_free_contents(node_t* node)
{
	free(node->contents);
}

void node_clear(node_t* starting_node)
{
	node_t* node;
	node_t* temp_node;
	node = starting_node;
	while (node != NULL && node != starting_node) {
		temp_node = node->next;
		free(node);
		node = temp_node;
	}
	node = starting_node;
	while (node != NULL && node != starting_node) {
		temp_node = node->prev;
		free(node);
		node = temp_node;
	}
}

void node_clear_contents(node_t* starting_node)
{
	node_t* node;
	node = starting_node;
	while (node != NULL && node != starting_node) {
		free(node->contents);
		node = node->next;
	}
	node = starting_node;
	while (node != NULL && node != starting_node) {
		free(node->contents);
		node = node->next;
	}
}

/*
node_t* node_next(node_t* node);
node_t* node_prev(node_t* node);
void* node_peek(node_t* node); 

void node_tweak(node_t* node, void* contents);
*/

node_t* node_append(node_t* node, void* new_contents)
{
	node_t* new_node;
	if (node == NULL)
		return node_new(NULL, NULL, new_contents);
	new_node = node_new(node, node->next, new_contents);
	if (node->next != NULL)
		node->next->prev = new_node;
	node->next = new_node;
	return new_node;
}

node_t* node_prepend(node_t* node, void* new_contents)
{
	node_t* new_node;
	if (node == NULL)
		return node_new(NULL, NULL, new_contents);
	new_node = node_new(node->prev, node, new_contents);
	if (node->prev != NULL)
		node->prev->next = new_node;
	node->prev = new_node;
	return new_node;
}

void* node_unlink(node_t* node)
{
	void* contents = node->contents;
	if (node->prev != NULL)
		node->prev->next = node->next;
	if (node->next != NULL)
		node->next->prev = node->prev;
	free(node);
	return contents;
}

char node_iterate(node_t** node, void** contents)
{
	if (*node == NULL)
		return 0;
	if (contents != NULL)
		*contents = (*node)->contents;
	*node = (*node)->next;
	return 1;
}

char node_retract(node_t** node, void** contents)
{
	if (*node == NULL)
		return 0;
	if (contents != NULL)
		*contents = (*node)->contents;
	*node = (*node)->next;
	return 1;
}

node_t* node_find_head(node_t* starting_node)
{
	node_t* node = starting_node;
	if (node == NULL)
		return NULL;
	while (node->prev != NULL) {
		node = node->prev;
		if (node == starting_node)
			return NULL;
	}
	return node;
}

node_t* node_find_tail(node_t* starting_node)
{
	node_t* node = starting_node;
	if (node == NULL)
		return NULL;
	while (node->next != NULL) {
		node = node->next;
		if (node == starting_node)
			return NULL;
	}
	return node;
}

node_t* node_push_head(node_t* starting_node, void* new_contents)
{
	return node_prepend(node_find_head(starting_node), new_contents);
}

node_t* node_push_tail(node_t* starting_node, void* new_contents)
{
	return node_append(node_find_tail(starting_node), new_contents);
}

node_t* node_pop_head(node_t* starting_node)
{
	return node_unlink(node_find_head(starting_node));
}

node_t* node_pop_tail(node_t* starting_node)
{
	return node_unlink(node_find_tail(starting_node));
}

uint node_find_size(node_t* starting_node)
{
	uint size = 0;
	node_t* node;

	node = starting_node;
	while (node != NULL) {
		size++;
		node = node->next;
		if (node == starting_node)
			break;
	}

	node = starting_node;
	while (node != NULL) {
		size++;
		node = node->prev;
		if (node == starting_node)
			break;
	}

	return size;
}

node_t* node_validate(node_t* node)
{
	return NULL; /* TODO */
}

