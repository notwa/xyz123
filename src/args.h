/*	args.h - argument handling
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

#ifndef ARGS_H_
#define ARGS_H_

#include "macros.h"
#include "list.h"

/* TODO: rewrite definitions */

/** Define these! */
extern void args_print_info();
extern void args_print_usage();
extern void args_print_help_suffix();

/** Automatically set to the string used
 *  to refer to the program; argv[0]. */
char* args_program_name;

/** TODO
 *  Switches are to be created with args_new_switch,
 *  then completed by you. */
typedef struct args_switch_s {
	/** A single character to match against.
	 *  Can be NULL if none is wanted.
	 *  Internally prepends a '-'. */
	char shortname;

	/** A string to match against.
	 *  Prepend your own '--' if desired. */
	char* longname;

	/** The description to print when
	 *  args_print_help is called. */
	char* description;

	/** The function to call when an argument
	 *  matches shortname or longname. */
	void (*function)(char* arg);
} args_switch_t;

/** Adds a switch to the list of switches to handle. */
void args_push_switch(args_switch_t*);

/** Retrieves the next argument.
 *  For use within a switch function. */
char* args_poll();

/** If this function is set, any argument that doesn't
 *  matches as a switch or generic will be sent to this. */
void (*args_fallback_handler)(char* arg);

/** Call this with main's argc and argv after setting up
 *  switches to begin argument handling. */
void args_handle(int argc, char* argv[]);

/** Frees any memory used and resets many variables. 
 *  You generally don't need to call this unless you have
 *  no memory to spare after calling args_handle. */
void args_reset();

/** Exactly as they sound. */
void args_print_help();
void args_print_switches();

#endif

