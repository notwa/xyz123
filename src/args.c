/* args.c - argument handling
 * Copyright (C) 2012 Connor Olding
 *
 * This program is licensed under the terms of the GNU General Public
 * License (version 3), and is distributed without any warranty.
 * You should have received a copy of the license in the file COPYING.
 * If not, visit http://gnu.org/licenses/ to obtain one.
 */

#include "args.h"

static node_t* args = NULL;
static node_t* switches = NULL;

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
	node_push_tail(switches, switch_);
}

static char is_blank(char* s)
{
	return s == NULL || s[0] == '\0' || s[0] == ' ';
}

static char is_shortname(char *s)
{
	return !is_blank(s) && s[0] == '-' && s[1] != '\0' && s[1] != '-';
}

void args_print_switches()
{
	node_t* iter = switches;
	args_switch_t* switch_;
	while (node_iterate(&iter, (void**) &switch_)) {
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
	puts("");
	args_print_switches();
	args_print_help_suffix();
	exit(0);
}

static void check_bounds()
{
	if (args->contents != NULL)
		return;

	args_print_usage();
	/*	FIXME
	if (current_arg - 1 == previous_switch)
		fprintf(stderr, "%s requires an argument.\n",
			previous_switch_name);
	*/
	fprintf(stderr, "%s requires another argument.\n",
			previous_switch_name);
	exit(1);
}

char* args_poll()
{
	char* arg = NULL;
	check_bounds();
	node_iterate(&args, (void**) &arg);
	arg += switch_subpos;
	switch_subpos = 0;
	return arg;
}

static char* poll_for_switch()
{
	static char short_switch[3] = "- ";
	char* arg = NULL;
	check_bounds();
	arg = args->contents;

	if (!is_shortname(arg)) {
		node_iterate(&args, (void**) &arg);
		return arg;
	}

	/* If arg is a list of short switches (eg. du -bad1),
	 * then yield a string for each switch (eg. -b -a -d -1).
	 * Note that short_switch is static and must be copied to persist
	 * across argument polls. */
	if (switch_subpos == 0)
		switch_subpos++;
	short_switch[1] = arg[switch_subpos];
	switch_subpos++;
	if (arg[switch_subpos] == '\0') {
		switch_subpos = 0;
		node_iterate(&args, NULL);
	}
	return short_switch;
}

static void find_switch(node_t** iter, char* name)
{
	static char short_switch[3] = "--";
	args_switch_t* switch_;
	while (node_iterate(iter, (void**) &switch_)) {
		short_switch[1] = switch_->shortname;
		if (!strcmp(short_switch, name))
			break;
		if (!is_blank(switch_->longname)
		&& !strcmp(switch_->longname, name))
			break;
	}
}

static void fill_args(int argc, char* argv[])
{
	int i;
	node_t* tail;
	args = node_new(NULL, NULL, argv[0]);
	tail = args;
	for (i = 1; i < argc; i++)
		tail = node_append(tail, argv[i]);
}

void args_handle(int argc, char* argv[])
{
	fill_args(argc, argv);

	args_program_name = args_poll();

	while (args != NULL) {
		node_t* iter = switches;
		char* name;

		name = poll_for_switch();
		previous_switch_name = name;

		find_switch(&iter, name);
		if (iter == NULL) {
			args_fallback_handler(name);
		} else {
			args_switch_t* switch_ = iter->contents;
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
		node_clear(switches);
}

