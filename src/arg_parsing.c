#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arg_parsing.h"

void print_usage() {
    printf("Usage: snowflake num_particles\n\n");
    printf("  -o --output   Output filename\n");
    printf("                Default is out/output.tga\n\n");
    exit(0);
}

void print_missing_argument(char *arg) {
    printf("Missing argument %s\n\n", arg);
    print_usage();
}

void print_not_enough_paramters(char *arg, int num_params_expected, int num_params_found) {
    printf("Argument %s expected %d parameters but found %d\n\n",
        arg, num_params_expected, num_params_found);
    print_usage();
}

void print_unrecognised_argument(char *arg) {
    printf("Unrecognised argument %s\n\n", arg);
    print_usage();
}

arg_options parse_args(int argc, char **argv) {
    if (argc < 2) {
        print_missing_argument("num_particles");
    }

    int num_particles;
    sscanf(argv[1], "%d", &num_particles);

    char *output = "out/output.tga";

    int argi = 2;
    while (argi < argc) {
        if (strcmp("--output", argv[argi]) == 0 ||
            strcmp("-o", argv[argi]) == 0)
        {
            if (argi + 1 >= argc) {
                print_not_enough_paramters(argv[argi], 1, argc - argi - 1);
            }
            output = argv[argi+1];
            argi += 2;
        }
        else {
            print_unrecognised_argument(argv[argi]);
        }
    }

    arg_options args;
    args.num_particles = num_particles;
    args.output = output;

    return args;
}
