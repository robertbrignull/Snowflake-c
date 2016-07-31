#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "algo.h"

#include "arg_parsing.h"

void print_usage() {
    printf("Usage: snowflake mode [args...]\n");
    printf("       snowflake --help\n");
    printf("  where mode is one gen, render, or bsp_test\n\n");

    printf("Mode gen\n");
    printf("  generates a snowflake.\n\n");
    printf("  Required argumens:\n");
    printf("    -n --num_particles    The number of particles to simulate\n\n");
    printf("  Optional argumens:\n");
    printf("    -o --output           Output the snowflake as text file\n");
    printf("                          Default is out/output.flake\n\n");

    printf("Mode render\n");
    printf("  render a snowflake from a previous run.\n\n");
    printf("  Required argumens:\n");
    printf("    -i --input            Input of a previous snowflake run as a text file\n\n");
    printf("  Optional argumens:\n");
    printf("    -o --output           Output filename for image\n");
    printf("                          Default is out/output.tga\n");
    printf("    -c --colorize         Show pixel age through color\n");
    printf("                          Default is no color\n\n");

    printf("Mode bsp_test\n");
    printf("  runs tests on the BSP implementation.\n\n");
    printf("  no arguments\n\n");

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
    return ((long_arg != 0 && strcmp(long_arg, actual_arg) == 0) ||
            (short_arg != 0 && strcmp(short_arg, actual_arg) == 0));
}

arg_options *parse_args(int argc, char **argv) {
    if (argc >= 2 && arg_matches(argv[1], "--help", "-h")) {
        print_usage();
    }

    arg_options *args = (arg_options*) malloc(sizeof(arg_options));
    CHECK_MEM(args);

    if (argc < 2) {
        print_missing_argument("mode");
    }

    if (arg_matches(argv[1], "gen", 0)) { // SNOWFLAKE_GEN
        args->mode = SNOWFLAKE_GEN;

        // required args
        args->gen.num_particles = 0;
        int num_particles_set = 0;

        // optional args
        char *default_output = "out/output.flake";
        args->gen.output = (char*) malloc(strlen(default_output) + 1);
        CHECK_MEM(args->gen.output);
        strcpy(args->gen.output, default_output);

        int argi = 2;
        while (argi < argc) {
            if (arg_matches(argv[argi], "--num-particles", "-n")) {
                check_enough_parameters(argv[argi], argc, argi, 1);
                args->gen.num_particles = atoi(argv[argi+1]);
                num_particles_set = 1;
                argi += 2;
            }
            else if (arg_matches(argv[argi], "--output", "-o")) {
                check_enough_parameters(argv[argi], argc, argi, 1);
                args->gen.output =
                    (char*) realloc(args->gen.output, strlen(argv[argi+1]) + 1);
                CHECK_MEM(args->gen.output);
                strcpy(args->gen.output, argv[argi+1]);
                argi += 2;
            }
            else {
                print_unrecognised_argument(argv[argi]);
            }
        }

        if (!num_particles_set) {
            print_missing_argument("--num-particles");
        }
    }
    else if (arg_matches(argv[1], "render", 0)) { // RENDER
        args->mode = RENDER;

        // required args
        args->render.input = 0;
        int input_set = 0;

        // optional args
        char *default_output = "out/output.tga";
        args->render.output = (char*) malloc(strlen(default_output) + 1);
        CHECK_MEM(args->render.output);
        strcpy(args->render.output, default_output);

        args->render.colorize = 0;

        int argi = 2;
        while (argi < argc) {
            if (arg_matches(argv[argi], "--output", "-o")) {
                check_enough_parameters(argv[argi], argc, argi, 1);
                args->render.output =
                    (char*) realloc(args->render.output, strlen(argv[argi+1]) + 1);
                CHECK_MEM(args->render.output);
                strcpy(args->render.output, argv[argi+1]);
                argi += 2;
            }
            else if (arg_matches(argv[argi], "--input", "-i")) {
                check_enough_parameters(argv[argi], argc, argi, 1);
                args->render.input =
                    (char*) realloc(args->render.input, strlen(argv[argi+1]) + 1);
                CHECK_MEM(args->render.input);
                strcpy(args->render.input, argv[argi+1]);
                input_set = 1;
                argi += 2;
            }
            else if (arg_matches(argv[argi], "--colorize", "-c")) {
                args->render.colorize = 1;
                argi += 1;
            }
            else {
                print_unrecognised_argument(argv[argi]);
            }
        }

        if (!input_set) {
            print_missing_argument("--log");
        }
    }
    else if (arg_matches(argv[1], "bsp_test", 0)) { // BSP_TEST
        args->mode = BSP_TEST;
    }
    else {
        print_unrecognised_argument(argv[1]);
    }

    return args;
}

void free_args(arg_options *args) {
    if (args->mode == SNOWFLAKE_GEN) {
        if (args->gen.output != 0) {
            free(args->gen.output);
        }
    }
    else if (args->mode == RENDER) {
        if (args->render.output != 0) {
            free(args->render.output);
        }
        if (args->render.input != 0) {
            free(args->render.input);
        }
    }
    free(args);
}