#pragma once

typedef enum program_mode {SNOWFLAKE_GEN, RENDER, TESTS} program_mode;

typedef enum symmetry_type_enum {NONE, ROTATIONAL, FULL} symmetry_type_enum;

typedef enum flake_impl { BSP, LINEAR } flake_impl;

typedef struct arg_options_gen {
    int num_particles;
    char *output;
    flake_impl impl;
    int symmetry_degree;
    symmetry_type_enum symmetry_type;
    int silent;
} arg_options_gen;

typedef struct arg_options_render {
    char *output;
    char *input;
    int colorize;
    int movie;
    int num_frames;
    int silent;
} arg_options_render;

typedef struct arg_options_tests {
    flake_impl impl;
} arg_options_tests;

typedef struct arg_options {
    program_mode mode;
    union {
        arg_options_gen gen;
        arg_options_render render;
        arg_options_tests tests;
    };
} arg_options;

arg_options *parse_args(int argc, char **argv);

void free_args(arg_options *args);
