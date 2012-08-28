/* xyz123 - xyz <-> gif image converter
 * Copyright (C) 2012 Connor Olding
 *
 * This program is licensed under the terms of the MIT License, and
 * is distributed without any warranty.  You should have received a
 * copy of the license along with this program; see the file LICENSE.
 */

#include "macros.h"
#include "buffs.h"
#include "node.h"
#include "args.h"
#include "image.h"
#include "gif.h"
#include "xyz.h"

enum {
	BUFFER_SIZE = 16 * 1024
};

static buffs_t* input;
static buffs_t* output;

static node_t* inputs_head = NULL;
static node_t* inputs_tail = NULL;
static char* desired_output_path = NULL;
static char force_gif = 0;
static char silent = 0;

void args_print_info()
{
	puts("xyz123 - xyz to gif image converter");
}

void args_print_usage()
{
	printf("Usage: %s FILE... [-g] [-o FILE]\n",
		args_program_name);
}

void args_print_help_suffix()
{
	puts("\n\
FILE may be '-' to specify stdin/stdout.\n\
If an output path isn't specified, each input will\n\
adopt an output path with the appropriate extension.");
}

static void set_input(char* arg)
{
	inputs_tail = node_push_tail(inputs_tail, arg);
	if (inputs_head == NULL)
		inputs_head = inputs_tail;
}

static void set_output(char* arg)
{
	desired_output_path = args_poll();
}

static void set_gif(char* arg)
{
	force_gif = 1;
}

static void set_silent(char* arg)
{
	silent = 1;
}

args_switch_t h = {
	'h',"--help","          display this text",
	args_print_help };
args_switch_t o = {
	'o',"--output","FILE    write to FILE",
	set_output };
args_switch_t b = {
	'g',"--gif","           force inputs to be treated as gifs",
	set_gif };
args_switch_t s = {
	's',"--silent","        don't print output filenames",
	set_silent };

static void setup_switches()
{
	inputs_head = NULL;
	inputs_tail = NULL;
	args_push_switch(&h);
	args_push_switch(&o);
	args_push_switch(&b);
	args_push_switch(&s);

	args_fallback_handler = set_input;
}

static char* find_last_slash(char* path)
{
	char* fslash = strrchr(path, '/');
	char* bslash = strrchr(path, '\\');
	if (bslash == NULL)
		return fslash;
	if (fslash == NULL)
		return bslash;
	return (fslash < bslash) ? fslash : bslash;
}

static char* find_dot(char* path)
{
	char* dot = strrchr(path, '.');
	char* slash;
	if (dot == NULL)
		return NULL;
	slash = find_last_slash(path);
	if (dot > slash)
		return dot;
	return NULL;
}

static char check_if_gif(char* input_path)
{
	char* dot;
	if (force_gif)
		return 1;
	dot = find_dot(input_path);
	if (dot == NULL || strlen(dot) < 2)
		return 0;
	if (!strcmp(dot + 1, "gif"))
		return 1;
	return 0;
}

static char* generate_path(char* input_path, char is_gif)
{
	int length;
	char* path;
	char* dot;

	if (desired_output_path != NULL)
		return desired_output_path;

	/* allocate with room for extension */
	length = strlen(input_path) + 5;
	path = CALLOC(char, length);

	strcpy(path, input_path);

	/* remove input extension if it exists */
	dot = find_dot(path);
	if (dot != NULL)
		*dot = '\0';

	strcat(path, ".");
	strcat(path, (is_gif) ? "xyz" : "gif");
	return path;
}

static int read_image(image_t* image, char* input_path, char is_gif)
{
	int status;
	buffs_open_input(input, input_path);
	if (is_gif)
		status = gif_read(image, input->stream);
	else
		status = xyz_read(image, input->stream);
	buffs_close(input);
	return status;
}

static int write_image(image_t* image, char* output_path, char is_gif)
{
	int status;
	buffs_open_output(output, output_path);
	if (is_gif)
		status = xyz_write(image, output->stream);
	else
		status = gif_write(image, output->stream);
	buffs_close(output);
	return status;
}

static void convert(char* input_path)
{
	image_t image;
	char is_gif = check_if_gif(input_path);
	char* output_path;
	int status = 0;

	if ((status = read_image(&image, input_path, is_gif))) {
		fprintf(stderr, "Couldn't read %s: %i\n",
			input_path, status);
		return;
	}

	output_path = generate_path(input_path, is_gif);

	if ((status = write_image(&image, output_path, is_gif))) {
		fprintf(stderr, "Couldn't write %s: %i\n",
			output_path, status);
	} else {
		if (!silent)
			puts(output_path);
	}

	free(image.palette);
	free(image.pixels);
	if (desired_output_path == NULL)
		free(output_path);
}

static void convert_inputs()
{
	char* input_path;
	node_t* inputs = inputs_head;
	int input_count = node_find_size(inputs);

	if (input_count == 0) {
		args_print_usage();
		fprintf(stderr, "An input must be specified."
			" Try --help for information.\n");
		exit(1);
	}
	if (desired_output_path != NULL && input_count > 1) {
		args_print_usage();
		fprintf(stderr, "An output cannot be specified"
			" with multiple inputs.\n");
		exit(1);
	}

	input = new_buffs(BUFFER_SIZE);
	output = new_buffs(BUFFER_SIZE);

	while (node_iterate(&inputs, (void**) &input_path))
		convert(input_path);

	node_clear(inputs_head);
	inputs_tail = NULL;
	free_buffs(input);
	free_buffs(output);
}

int main(int argc, char** argv)
{
	setup_switches();
	args_handle(argc, argv);
	convert_inputs();

	return 0;
}

