/* xyz123 - xyz <-> gif image converter
 * Copyright (C) 2012 Connor Olding
 *
 * This program is licensed under the terms of the MIT License, and
 * is distributed without any warranty.  You should have received a
 * copy of the license along with this program; see the file LICENSE.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "unsigned.h"
#include "args.h"
#include "image.h"
#include "gif.h"
#include "xyz.h"
#include "util.h"

typedef struct string_node_s string_node;
struct string_node_s {
	char *s;
	string_node *next;
};

static string_node *input_node = NULL;
static int inputs = 0;
static char* desired_output_path = NULL;
static char force_gif = 0;
static char silent = 0;

static const char help1[] = "\
xyz123 - xyz to gif image converter\n\
\n\
  <files...>        open files as inputs\n\
  -h                display this text\n\
  -s                don't print output filenames\n\
  -g                force inputs to be treated as gifs\n\
  -o                write to FILE\n\
\n\
";
static const char help2[] = "\
FILE may be '-' to specify stdin/stdout.\n\
If an output path isn't specified, each input will\n\
adopt an output path with the appropriate extension.\n\
";

static char *check_next(char flag, char *next) {
	if (!next)
		die("-%c requires another argument\n", flag);
	return next;
}

static void handle_flag(char flag, char *(*nextarg)())
{
	char *next;
	switch (flag) {
	case 'h':
		printf(help1);
		printf(help2);
		exit(0);
	case 'g':
		force_gif = 1;
		return;
	case 's':
		silent = 1;
		return;
	case 'o':
		next = check_next(flag, nextarg());
		desired_output_path = next;
		break;
	default:
		die("Unknown flag: -%c\n", flag);
	}
}

static void add_input(char *arg)
{
	static string_node *last = NULL;
	string_node *n = callocs(sizeof(string_node));

	n->s = arg;
	inputs++;

	if (!input_node)
		input_node = n;
	else
		last->next = n;
	last = n;
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
	path = callocs(length);

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
	FILE *input = fopen(input_path, "r");
	if (input == NULL) {
		perror(input_path);
		exit(EXIT_FAILURE);
	}
	if (is_gif)
		status = gif_read(image, input);
	else
		status = xyz_read(image, input);
	fclose(input);
	return status;
}

static int write_image(image_t* image, char* output_path, char is_gif)
{
	int status;
	FILE *output = fopen(output_path, "w");
	if (output == NULL) {
		perror(output_path);
		exit(EXIT_FAILURE);
	}
	if (is_gif)
		status = xyz_write(image, output);
	else
		status = gif_write(image, output);
	fclose(output);
	return status;
}

static int convert(char* input_path)
{
	image_t image;
	char is_gif = check_if_gif(input_path);
	char* output_path;
	int status = read_image(&image, input_path, is_gif);

	if (status)
		return status;

	output_path = generate_path(input_path, is_gif);

	status = write_image(&image, output_path, is_gif);
	if (!status && !silent)
		puts(output_path);

	free(image.palette);
	free(image.pixels);
	if (desired_output_path == NULL)
		free(output_path);
	return status;
}

static void convert_inputs()
{
	string_node *n;

	if (inputs == 0)
		die("An input must be specified. Try -h for information.\n");
	if (desired_output_path != NULL && inputs > 1)
		die("An output cannot be specified with multiple inputs.\n");

	for (n = input_node; n; n = n->next) {
		int status = convert(n->s);
		if (status)
			fprintf(stderr, "Couldn't convert %s: %i\n",
			    n->s, status);
		free(n);
	}
}

int main(int argc, char** argv)
{
	args_parse(argc, argv, handle_flag, add_input);
	convert_inputs();

	return 0;
}

