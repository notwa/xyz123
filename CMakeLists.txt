cmake_minimum_required(VERSION 2.6)

project(xyz123)

set(CMAKE_C_FLAGS "-ansi -pedantic -Wall -Werror")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${xyz123_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${xyz123_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${xyz123_BINARY_DIR}/bin")

add_subdirectory(src)

