/*	args.c - argument handling
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

#include "args.h"
#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static list_t* args = NULL;
static list_t* switches = NULL;

char* args_program_name = NULL;
static char* previous_switch_name = NULL;
static int switch_subpos = 0;

static void default_fallback(char* arg)
{
	fprintf(stderr, "Unknown switch: %s\n", arg);
	args_print_usage();
	exit(1);
}

void (*args_fallback_handler)(char* arg) = default_fallback;

void args_push_switch(args_switch_t* switch_)
{
	if (switches == NULL)
		switches = new_list();
	list_push(switches, switch_);
}

static char is_blank(char* s)
{
	if (s == NULL || s[0] == '\0' || s[0] == ' ')
		return 1;
	return 0;
}

static char is_shortname(char *s)
{
	if (!is_blank(s) && s[0] == '-' && s[1] != '\0' && s[1] != '-')
		return 1;
	return 0;
}

void args_print_switches()
{
	args_switch_t* switch_;
	list_restart(switches);
	while ((switch_ = list_iterate(switches)) != NULL) {
		printf("  ");
		if (switch_->shortname)
			printf("-%c", switch_->shortname);
		else
			printf("  "); 
		if (switch_->shortname && switch_->longname)
			printf(", ");
		else
			printf("  ");
		if (switch_->longname)
			printf("%s", switch_->longname);
		if (switch_->longname && switch_->description)
			printf(" ");
		if (switch_->description)
			printf("%s", switch_->description);
		printf("\n");
	}
}

void args_print_help()
{
	args_print_info();
	args_print_usage();
	args_print_switches();
	args_print_help_suffix();
	exit(0);
}

static void check_bounds()
{
	if (list_peek(args) == NULL) {
		args_print_usage();
		/* FIXME
		if (current_arg - 1 == previous_switch)
			fprintf(stderr, "%s requires an argument.\n",
				previous_switch_name);
		*/
		fprintf(stderr, "%s requires another argument.\n",
				previous_switch_name);
		exit(1);
	}
}

char* args_poll()
{
	char* arg = NULL;
	check_bounds();
	arg = list_pop(args);
	arg += switch_subpos * sizeof(char);
	switch_subpos = 0;
	return arg;
}

static char* poll_for_switch()
{
	static char short_switch[3] = "--";
	char* arg = NULL;
	check_bounds();
	arg = list_peek(args);
	if (!is_shortname(arg)) {
		list_pop(args);
		return arg;
	}
	if (switch_subpos == 0)
		switch_subpos++;
	short_switch[1] = arg[switch_subpos];
	switch_subpos++;
	if (arg[switch_subpos] == '\0') {
		switch_subpos = 0;
		list_pop(args);
	}
	return short_switch;
}

static int get_index(char* name)
{
	static char short_switch[3] = "--";
	int i = 0;
	args_switch_t* switch_;
	list_restart(switches);
	while ((switch_ = list_iterate(switches)) != NULL) {
		short_switch[1] = switch_->shortname;
		if (!strcmp(short_switch, name))
			break;
		if (!is_blank(switch_->longname)
		&& !strcmp(switch_->longname, name))
			break;
		i++;
	}
	if (i == list_size(switches))
		i = -1;
	return i;
}

static void fill_args(int argc, char* argv[])
{
	int i;
	for (i = 0; i < argc; i++)
		list_push(args, argv[i]);
}

void args_handle(int argc, char* argv[])
{
	args = new_list();
	fill_args(argc, argv);

	args_program_name = args_poll();

	while (list_peek(args) != NULL) {
		char* name;
		int index;

		name = poll_for_switch();
		previous_switch_name = name;

		index = get_index(name);
		if (index < 0) {
			args_fallback_handler(name);
		} else {
			args_switch_t* switch_ = list_inspect(switches);
			if (switch_->function)
				switch_->function(name);
		}
	}
}

void args_reset()
{
	args_program_name = NULL;
	previous_switch_name = NULL;
	switch_subpos = 0;
	args_fallback_handler = default_fallback;

	if (switches != NULL)
		list_clear(switches);
}

