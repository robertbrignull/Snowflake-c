#pragma once

typedef enum program_mode {SNOWFLAKE_GEN, BSP_TEST} program_mode;

typedef struct arg_options {
    program_mode mode;
    union {
        struct { // SNOWFLAKE_GEN
            int num_particles;
            char *image_output;
            char *log_output;
        };
    };
} arg_options;

arg_options *parse_args(int argc, char **argv);

void free_args(arg_options *args);
