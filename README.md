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

TODO
----

- Add gif reading.

- Add "opaque" and "silent" switches.

- Write a Usage section in the readme.

- Add "--" handling to args.

- Hardcode "--" prefixes for long switches.

- Fix list\_unlink.

- Write and rewrite some comments

Notes
-----

The name "xyz123" pokes fun at the genericness of the name of the format it
works with. Mind you, I probably couldn't come up with anything better.

