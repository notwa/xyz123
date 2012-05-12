/*	xyz123 - xyz to BMP image converter
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

static buffs_t input, output;

static list_t* inputs = NULL;
static char* desired_output_path = NULL;

void args_print_info()
{
	puts("xyz123 - xyz to BMP image converter");
}

void args_print_usage()
{
	printf("Usage: %s FILE [FILES...] [-o FILE]\n", args_program_name);
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

args_switch_t h = {
	'h',"--help","          display this text",
	args_print_help };
args_switch_t o = {
	'o',"--output","FILE    write to FILE as an image",
	set_output };

static void setup_switches()
{
	inputs = new_list();
	args_push_switch(&h);
	args_push_switch(&o);

	args_fallback_handler = set_input;
}

static char* get_last_slash(char* path)
{
	char* fslash = strrchr(path, '/');
	char* bslash = strrchr(path, '\\');
	if (bslash == NULL)
		return fslash;
	if (fslash == NULL)
		return bslash;
	return (fslash < bslash) ? fslash : bslash;
}

static void remove_extension(char* path)
{
	char* dot = strrchr(path, '.');
	char* slash;
	if (dot == NULL)
		return;
	slash = get_last_slash(path);
	if (dot > slash)
		*dot = '\0';
}

static char* generate_path(char* input_path)
{
	int length = strlen(input_path);
	/* allocate with room for ".bmp" */
	char* path = CALLOC(char, length + 4);

	strcpy(path, input_path);
	remove_extension(path);
	strcat(path, ".bmp");
	return path;
}

static void convert(char* input_path)
{
	image_t image;
	char* output_path = desired_output_path;
	if (output_path == NULL)
		output_path = generate_path(input_path);

	puts(input_path);
	puts(output_path);

	buffs_open(&input, input_path, "rb", stdin);
	buffs_open(&output, output_path, "wb", stdout);

	xyz_read(&image, input.stream);
	bmp_write(&image, output.stream);

	buffs_close(&input);
	buffs_close(&output);

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

	input_path = (char*) list_pop_front(inputs);
	while (input_path != NULL) {
		convert(input_path);
		input_path = (char*) list_pop_front(inputs);
	}
	free(inputs);
}

int main(int argc, char** argv)
{
	setup_switches(); 
	args_handle(argc, argv);
	convert_inputs();

	return 0;
}

