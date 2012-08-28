add_executable(xyz123 main.c args.c futil.c util.c gif.c xyz.c)

find_package(ZLIB REQUIRED)
find_package(GIF REQUIRED)

include_directories(${GIF_INCLUDE_DIR} ${ZLIB_INCLUDE_DIRS})
target_link_libraries(xyz123 ${GIF_LIBRARIES} ${ZLIB_LIBRARIES})

