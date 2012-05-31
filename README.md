xyz123
======

Converts RPG Maker's xyz image format to and from gifs.

Building
--------

### Prerequisites ###

[zlib][] and [giflib][].
These should be available in your distro's package manager.

[zlib]: http://zlib.net/
[giflib]: http://sourceforge.net/projects/giflib/

### Compilation ###

**GNU Make:** `make` or `make debug`, optionally `make install`

**CMake:** CMake files are provided for users that prefer to use it.
As always, it's recommended to do an [out-of-source build][oosb] with
CMake to avoid cluttering the source tree.

[oosb]: http://vtk.org/Wiki/CMake_FAQ#Out-of-source_build_trees

Usage
-----

	Usage: bin/xyz123 FILE... [-g] [-o FILE]

	  -h, --help           display this text
	  -o, --output FILE    write to FILE
	  -g, --gif            force inputs to be treated as gifs
	  -s, --silent         don't print output filenames

	FILE may be '-' to specify stdin/stdout.
	If an output path isn't specified, each input will
	adopt an output path with the appropriate extension.

TODO
----

- Add "--" handling to args.

- Hardcode "--" prefixes for long switches.

- Fix list\_unlink.

- Write and rewrite some comments

Notes
-----

The name "xyz123" pokes fun at the genericness of the name of the format it
works with. Mind you, I probably couldn't come up with anything better.

