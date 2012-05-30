cmake_minimum_required(VERSION 2.6)
include(CheckIncludeFiles)

project(xyz123)

if (CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -Wall -Werror")
endif ()

check_include_files(stdint.h HAVE_STDINT_H)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${xyz123_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${xyz123_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${xyz123_BINARY_DIR}/bin")

add_subdirectory(src)

