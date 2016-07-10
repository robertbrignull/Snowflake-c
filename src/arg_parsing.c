#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "algo.h"

#include "arg_parsing.h"

void print_usage() {
    printf("Usage: snowflake mode (--num-particles 100000) [--output out/output.tga]\n");
    printf("       snowflake --help\n");
    printf("  where mode is one gen, or bsp_test\n\n");

    printf("Mode gen\n");
    printf("  generates a snowflake.\n\n");
    printf("  Required argumens:\n");
    printf("    -n --num_particles    The number of particles to simulate\n\n");
    printf("  Optional argumens:\n");
    printf("    -o --output   Output filename\n");
    printf("                  Default is out/output.tga\n\n");

    printf("Mode bsp_test\n");
    printf("  runs test on the BSP implementation.\n\n");
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
        char *default_image_output = "out/output.tga";
        args->gen.image_output = (char*) malloc(strlen(default_image_output) + 1);
        CHECK_MEM(args->gen.image_output);
        strcpy(args->gen.image_output, default_image_output);

        args->gen.log_output = 0;

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
                args->gen.image_output = (char*) realloc(args->gen.image_output,
                    strlen(argv[argi+1]) + 1);
                CHECK_MEM(args->gen.image_output);
                strcpy(args->gen.image_output, argv[argi+1]);
                argi += 2;
            }
            else if (arg_matches(argv[argi], "--log", "-l")) {
                check_enough_parameters(argv[argi], argc, argi, 1);
                args->gen.log_output = (char*) realloc(args->gen.log_output,
                    strlen(argv[argi+1]) + 1);
                CHECK_MEM(args->gen.log_output);
                strcpy(args->gen.log_output, argv[argi+1]);
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
        args->render.log_input = 0;
        int log_input_set = 0;

        // optional args
        char *default_image_output = "out/output.tga";
        args->render.image_output = (char*) malloc(strlen(default_image_output) + 1);
        CHECK_MEM(args->render.image_output);
        strcpy(args->render.image_output, default_image_output);

        int argi = 2;
        while (argi < argc) {
            if (arg_matches(argv[argi], "--output", "-o")) {
                check_enough_parameters(argv[argi], argc, argi, 1);
                args->render.image_output = (char*) realloc(args->render.image_output,
                    strlen(argv[argi+1]) + 1);
                CHECK_MEM(args->render.image_output);
                strcpy(args->render.image_output, argv[argi+1]);
                argi += 2;
            }
            else if (arg_matches(argv[argi], "--log", "-l")) {
                check_enough_parameters(argv[argi], argc, argi, 1);
                args->render.log_input = (char*) realloc(args->render.log_input,
                    strlen(argv[argi+1]) + 1);
                CHECK_MEM(args->render.log_input);
                strcpy(args->render.log_input, argv[argi+1]);
                log_input_set = 1;
                argi += 2;
            }
            else {
                print_unrecognised_argument(argv[argi]);
            }
        }

        if (!log_input_set) {
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
        if (args->gen.image_output != 0) {
            free(args->gen.image_output);
        }
        if (args->gen.log_output != 0) {
            free(args->gen.log_output);
        }
    }
    else if (args->mode == RENDER) {
        if (args->render.image_output != 0) {
            free(args->render.image_output);
        }
        if (args->render.log_input != 0) {
            free(args->render.log_input);
        }
    }
    free(args);
}