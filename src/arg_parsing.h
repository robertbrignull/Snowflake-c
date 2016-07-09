#pragma once

typedef struct arg_options {
    int num_particles;
} arg_options;

arg_options parse_args(int argc, char **argv);
