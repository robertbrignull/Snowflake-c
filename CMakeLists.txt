cmake_minimum_required(VERSION 3.5)
project(Snowflake)

set(SOURCE_FILES src/algo.c src/arg_parsing.c src/bsp.c src/bsp_test.c src/render_log.c src/snowflake.c src/snowflake_gen.c src/flake_log.h src/flake_log.c src/write_png.h src/write_png.c)
include_directories(src)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -O3 -ggdb -gdwarf-2")

add_executable(snowflake ${SOURCE_FILES})
target_link_libraries(snowflake m png)
