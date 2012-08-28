/* node.h - one-dimensional linked nodes to act as linked lists
 * Copyright (C) 2012 Connor Olding
 *
 * This program is licensed under the terms of the MIT License, and
 * is distributed without any warranty.  You should have received a
 * copy of the license along with this program; see the file LICENSE.
 */

typedef struct node_s node_t;

struct node_s {
	node_t* prev;
	node_t* next;
	void* contents;
};

/* TODO: add copying methods */

/* Allocates and returns a new node. */
node_t* node_new_null();
node_t* node_new(node_t* prev, node_t* next, void* contents);

/* Frees a node and returns its contents.
 * node_clear and node_unlink are generally more suitable. */
void* node_free(node_t* node);

/* Frees every linked node. */
void node_clear(node_t* starting_node);

/* Frees the contents of every linked node.
 * Doesn't free the nodes themselves. */
void node_clear_contents(node_t* starting_node);

/* getters/setters *
node_t* node_next(node_t* node);
node_t* node_prev(node_t* node);
void* node_peek(node_t* node);
void node_tweak(node_t* node, void* contents);
*/

/* Adds and returns a new node after/before the node. */
node_t* node_append(node_t* node, void* new_contents);
node_t* node_prepend(node_t* node, void* new_contents);

/* Unlinks and frees a node, returning its contents. */
void* node_unlink(node_t* node);

/* TODO: describe me */
node_t* node_iterate(node_t** node, void** contents);
node_t* node_retract(node_t** node, void** contents);

/* Returns the head/tail of a list, or
 * NULL when a circular loop is detected. */
node_t* node_find_head(node_t* starting_node);
node_t* node_find_tail(node_t* starting_node);

/* Adds and returns a new node to the head/tail of a list.
 * Consider using node_append/prepend when suitable.
 * Doesn't work on loops. */
node_t* node_push_head(node_t* starting_node, void* new_contents);
node_t* node_push_tail(node_t* starting_node, void* new_contents);

/* Removes and returns the head/tail of a list.
 * Consider using node_unlink when suitable.
 * Doesn't work on loops. */
node_t* node_pop_head(node_t* starting_node);
node_t* node_pop_tail(node_t* starting_node);

/* Calculates the size of a list by counting forwards
 * and backwards from a node. Works on circular loops. */
uint node_find_size(node_t* starting_node);

/* Returns NULL when a list is properly linked,
 * otherwise returns the first node with a bad prev value.
 * Starts from the head, so this doesn't work on loops.
 * You shouldn't need to use this unless you've been
 * manually toying around with nodes. */
node_t* node_validate(node_t* node);

