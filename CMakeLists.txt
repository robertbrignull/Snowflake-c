cmake_minimum_required(VERSION 3.5)
project(Snowflake)

set(SOURCE_FILES src/algo.h src/args/arg_parsing.c src/args/arg_parsing.h src/data/bsp/bsp.c src/data/bsp/bsp.h src/data/bsp/bsp_test.c src/data/bsp/bsp_test.h src/data/flake.c src/data/flake.h src/data/linear/linear.c src/data/linear/linear.h src/render/render_log.c src/render/render_log.h src/snowflake.c src/gen/snowflake_gen.c src/gen/snowflake_gen.h src/log/flake_log.h src/log/flake_log.c src/render/write_png.h src/render/write_png.c)
include_directories(src)

if (DEFINED ENV{DATA_IMPL_LINEAR})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDATA_IMPL_LINEAR=$ENV{DATA_IMPL_LINEAR}")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Werror -O3 -ggdb -gdwarf-2 --short-enums")

add_executable(snowflake ${SOURCE_FILES})
target_link_libraries(snowflake m png)
