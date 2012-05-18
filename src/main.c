/*	xyz123 - xyz <-> gif image converter
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

#include "main.h"

enum {
	BUFFER_SIZE = 16 * 1024
};

static buffs_t* input;
static buffs_t* output;

static list_t* inputs = NULL;
static char* desired_output_path = NULL;
static char force_gif = 0;

void args_print_info()
{
	puts("xyz123 - xyz to BMP image converter");
}

void args_print_usage()
{
	printf("Usage: %s FILE [FILES...] [-b] [-o FILE]\n",
		args_program_name);
}

void args_print_help_suffix()
{
	puts("\n\
FILE may be '-' to use stdin/stdout for input/output.\n\
If no output is specified, it will default to the\n\
input's path with the appropriate extension.");
}

static void set_input(char* arg)
{
	list_push_back(inputs, arg);
}

static void set_output(char* arg)
{
	desired_output_path = args_poll();
}

static void set_gif(char* arg)
{
	force_gif = 1;
}

args_switch_t h = {
	'h',"--help","          display this text",
	args_print_help };
args_switch_t o = {
	'o',"--output","FILE    write to FILE as an image",
	set_output };
args_switch_t b = {
	'g',"--gif","           force inputs to be treated as gifs",
	set_gif };

static void setup_switches()
{
	inputs = new_list();
	args_push_switch(&h);
	args_push_switch(&o);
	args_push_switch(&b);

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

static int read(image_t* image, char* input_path, char is_gif)
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

static int write(image_t* image, char* output_path, char is_gif)
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

	if ((status = read(&image, input_path, is_gif))) {
		fprintf(stderr, "Couldn't read %s: %i\n",
			input_path, status);
		return;
	}

	output_path = generate_path(input_path, is_gif);

	if ((status = write(&image, output_path, is_gif)))
		fprintf(stderr, "Couldn't write %s: %i\n",
			output_path, status);
	else
		puts(output_path);

	if (desired_output_path == NULL)
		free(output_path);
}

static void convert_inputs()
{
	char* input_path;
	int input_count = list_size(inputs);

	if (input_count == 0) {
		args_print_usage();
		fprintf(stderr, "An input must be specified.\n");
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

	input_path = (char*) list_pop_front(inputs);
	while (input_path != NULL) {
		convert(input_path);
		input_path = (char*) list_pop_front(inputs);
	}

	free(inputs);
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

