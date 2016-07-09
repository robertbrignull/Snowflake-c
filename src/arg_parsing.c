#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "algo.h"

#include "arg_parsing.h"

void print_usage() {
    printf("Usage: snowflake (--num-particles 100000) [--output out/output.tga]\n\n");
    printf("Required argumens:\n");
    printf("  -n --num_particles    The number of particles to simulate\n\n");
    printf("Optional argumens:\n");
    printf("  -o --output   Output filename\n");
    printf("                Default is out/output.tga\n\n");
    exit(0);
}

void print_missing_argument(char *arg) {
    printf("Missing argument %s\n\n", arg);
    print_usage();
}

void print_unrecognised_argument(char *arg) {
    printf("Unrecognised argument %s\n\n", arg);
    print_usage();
}

void check_enough_parameters(char *arg, int argc, int argi, int num_params_expected) {
    if (argi + 1 >= argc) {
        printf("Argument %s expected %d parameters but found %d\n\n",
            arg, num_params_expected, argc - argi - 1);
        print_usage();
    }
}

int arg_matches(char *actual_arg, char *long_arg, char *short_arg) {
    return (strcmp(long_arg, actual_arg) == 0 ||
            strcmp(short_arg, actual_arg) == 0);
}

arg_options *parse_args(int argc, char **argv) {
    if (argc >= 2 && arg_matches(argv[1], "--help", "-h")) {
        print_usage();
    }

    arg_options *args = (arg_options*) malloc(sizeof(arg_options));
    CHECK_MEM(args);

    // required args
    args->num_particles = 0;
    int num_particles_set = 0;

    // optional args
    char *default_image_output = "out/output.tga";
    args->image_output = (char*) malloc(strlen(default_image_output) + 1);
    CHECK_MEM(args->image_output);
    strcpy(args->image_output, default_image_output);

    args->log_output = 0;

    int argi = 1;
    while (argi < argc) {
        if (arg_matches(argv[argi], "--num-particles", "-n")) {
            check_enough_parameters(argv[argi], argc, argi, 1);
            args->num_particles = atoi(argv[argi+1]);
            num_particles_set = 1;
            argi += 2;
        }
        else if (arg_matches(argv[argi], "--output", "-o")) {
            check_enough_parameters(argv[argi], argc, argi, 1);
            args->image_output = (char*) realloc(args->image_output,
                strlen(argv[argi+1]) + 1);
            CHECK_MEM(args->image_output);
            strcpy(args->image_output, argv[argi+1]);
            argi += 2;
        }
        else if (arg_matches(argv[argi], "--log", "-l")) {
            check_enough_parameters(argv[argi], argc, argi, 1);
            args->log_output = (char*) realloc(args->log_output,
                strlen(argv[argi+1]) + 1);
            CHECK_MEM(args->log_output);
            strcpy(args->log_output, argv[argi+1]);
            argi += 2;
        }
        else {
            print_unrecognised_argument(argv[argi]);
        }
    }

    if (!num_particles_set) {
        print_missing_argument("--num-particles");
    }

    return args;
}

void free_args(arg_options *args) {
    free(args->image_output);
    if (args->log_output != 0) {
        free(args->log_output);
    }
    free(args);
}