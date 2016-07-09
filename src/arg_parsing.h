#pragma once

typedef struct arg_options {
    int num_particles;
    char *output;
} arg_options;

arg_options parse_args(int argc, char **argv);
