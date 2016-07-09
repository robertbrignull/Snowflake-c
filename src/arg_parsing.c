#include <stdio.h>
#include <stdlib.h>

#include "arg_parsing.h"

arg_options parse_args(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: snowflake num_particles\n");
        exit(0);
    }

    int N;  // N is the max number of particles
    sscanf(argv[1], "%d", &N);

    arg_options args;
    args.num_particles = N;

    return args;
}
