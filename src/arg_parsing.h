#pragma once

typedef struct arg_options {
    int num_particles;
    char *image_output;
    char *log_output;
} arg_options;

arg_options *parse_args(int argc, char **argv);

void free_args(arg_options *args);
