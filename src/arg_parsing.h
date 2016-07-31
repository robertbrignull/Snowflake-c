#pragma once

typedef enum program_mode {SNOWFLAKE_GEN, RENDER, BSP_TEST} program_mode;

typedef struct arg_options_gen {
    int num_particles;
    char *output;
} arg_options_gen;

typedef struct arg_options_render {
    char *output;
    char *input;
    int colorize;
    int movie;
    int num_frames;
} arg_options_render;

typedef struct arg_options {
    program_mode mode;
    union {
        arg_options_gen gen;
        arg_options_render render;
    };
} arg_options;

arg_options *parse_args(int argc, char **argv);

void free_args(arg_options *args);
