#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "algo.h"

#include "args/arg_parsing.h"

void print_usage(int exit_code) {
    printf("Usage: snowflake mode [args...]\n");
    printf("       snowflake --help\n");
    printf("  where mode is one gen, render, or bsp_test\n\n");

    printf("Mode gen\n");
    printf("  generates a snowflake.\n\n");
    printf("  Required argumens:\n");
    printf("    -o --output           Output filename for snowflake data\n");
    printf("                          If already exists then will continue the flake already there\n\n");
    printf("  Optional argumens:\n");
    printf("    -n --num-particles    The number of particles to simulate\n");
    printf("                          Default is no limit\n");
    printf("    -d --symmetry-degree  The number of axis of symmetry to employ\n");
    printf("                          Default is 6\n");
    printf("    -t --symmetry-type    The type of symmetry to use. Options include:\n");
    printf("                          - none: no symmetry (default)\n");
    printf("                          - rotational: rotational symmetry\n");
    printf("                          - full: rotational and reflectional symmetry\n");
    printf("    -s --silent           Don't output progress\n\n");

    printf("Mode render\n");
    printf("  render a snowflake from a previous run.\n\n");
    printf("  Required argumens:\n");
    printf("    -i --input            Input filename of snowflake data\n\n");
    printf("  Optional argumens:\n");
    printf("    -o --output           Output filename for image\n");
    printf("                          Default is to add a .png extension to the input file\n");
    printf("    -c --colorize         Show pixel age through color\n");
    printf("                          Default is no color\n");
    printf("    -m --movie            Output all states as images instead of just the final flake\n");
    printf("                          Default is just the final frame\n");
    printf("                          If this is used then the output filename should be\n");
    printf("                            provided as a format string, e.g. out/output_%%d.png\n");
    printf("    -f --frames           The number of frames to produce if making a movie\n");
    printf("    -s --silent           Don't output progress\n\n");

    printf("Mode bsp_test\n");
    printf("  runs tests on the BSP implementation.\n\n");
    printf("  no arguments\n\n");

    exit(exit_code);
}

void print_missing_argument(char *arg) {
    printf("Missing argument %s\n\n", arg);
    print_usage(1);
}

void print_unrecognised_argument(char *arg) {
    printf("Unrecognised argument %s\n\n", arg);
    print_usage(1);
}

void check_enough_parameters(char *arg, int argc, int argi, int num_params_expected) {
    if (argi + 1 >= argc) {
        printf("Argument %s expected %d parameters but found %d\n\n",
            arg, num_params_expected, argc - argi - 1);
        print_usage(1);
    }
}

int arg_matches(char *actual_arg, char *long_arg, char *short_arg) {
    return ((long_arg != 0 && strcmp(long_arg, actual_arg) == 0) ||
            (short_arg != 0 && strcmp(short_arg, actual_arg) == 0));
}

char *replace_extension(char *original_str, char *new_extension) {
    size_t original_length = strlen(original_str);

    size_t extension_start = original_length;
    for (size_t i = 0; i < original_length; i++) {
        if (original_str[original_length - i] == '.') {
            extension_start = original_length - i;
            break;
        }
    }

    size_t extension_length = strlen(new_extension);
    size_t new_length = extension_start + extension_length + 2;
    char *new_str = malloc(new_length);
    CHECK_MEM(new_str);

    strncpy(new_str, original_str, extension_start);
    new_str[extension_start] = '.';
    strncpy(new_str + extension_start + 1, new_extension, extension_length);
    new_str[extension_start + extension_length + 1] = 0;

    return new_str;
}

arg_options *parse_args(int argc, char **argv) {
    if (argc >= 2 && arg_matches(argv[1], "--help", "-h")) {
        print_usage(0);
    }

    arg_options *args = (arg_options*) malloc(sizeof(arg_options));
    CHECK_MEM(args);

    if (argc < 2) {
        print_missing_argument("mode");
    }

    if (arg_matches(argv[1], "gen", 0)) { // SNOWFLAKE_GEN
        args->mode = SNOWFLAKE_GEN;

        // required args
        args->gen.output = 0;
        int output_set = 0;

        // optional args
        args->gen.num_particles = -1;
        args->gen.symmetry_degree = 6;
        args->gen.symmetry_type = NONE;
        args->gen.silent = 0;

        int argi = 2;
        while (argi < argc) {
            if (arg_matches(argv[argi], "--num-particles", "-n")) {
                check_enough_parameters(argv[argi], argc, argi, 1);
                args->gen.num_particles = atoi(argv[argi+1]);
                argi += 2;
            }
            else if (arg_matches(argv[argi], "--output", "-o")) {
                check_enough_parameters(argv[argi], argc, argi, 1);
                args->gen.output =
                    (char*) realloc(args->gen.output, strlen(argv[argi+1]) + 1);
                CHECK_MEM(args->gen.output);
                strcpy(args->gen.output, argv[argi+1]);
                output_set = 1;
                argi += 2;
            }
            else if (arg_matches(argv[argi], "--symmetry-degree", "-d")) {
                check_enough_parameters(argv[argi], argc, argi, 1);
                args->gen.symmetry_degree = atoi(argv[argi+1]);
                argi += 2;
            }
            else if (arg_matches(argv[argi], "--symmetry-type", "-t")) {
                check_enough_parameters(argv[argi], argc, argi, 1);
                if (arg_matches(argv[argi+1], "none", 0)) {
                    args->gen.symmetry_type = NONE;
                }
                else if (arg_matches(argv[argi+1], "rotational", 0)) {
                    args->gen.symmetry_type = ROTATIONAL;
                }
                else if (arg_matches(argv[argi+1], "full", 0)) {
                    args->gen.symmetry_type = FULL;
                }
                else {
                    print_unrecognised_argument(argv[argi+1]);
                }
                argi += 2;
            }
            else if (arg_matches(argv[argi], "--silent", "-s")) {
                args->gen.silent = 1;
                argi += 1;
            }
            else {
                print_unrecognised_argument(argv[argi]);
            }
        }

        if (!output_set) {
            print_missing_argument("--output");
        }
    }
    else if (arg_matches(argv[1], "render", 0)) { // RENDER
        args->mode = RENDER;

        // required args
        args->render.input = 0;
        int input_set = 0;

        // optional args
        args->render.output = 0;
        int output_set = 0;
        args->render.colorize = 0;
        args->render.movie = 0;
        args->render.num_frames = 0;
        int num_frames_set = 0;
        args->render.silent = 0;

        int argi = 2;
        while (argi < argc) {
            if (arg_matches(argv[argi], "--output", "-o")) {
                check_enough_parameters(argv[argi], argc, argi, 1);
                args->render.output =
                    (char*) realloc(args->render.output, strlen(argv[argi+1]) + 1);
                CHECK_MEM(args->render.output);
                strcpy(args->render.output, argv[argi+1]);
                output_set = 1;
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
            else if (arg_matches(argv[argi], "--movie", "-m")) {
                args->render.movie = 1;
                argi += 1;
            }
            else if (arg_matches(argv[argi], "--frames", "-f")) {
                check_enough_parameters(argv[argi], argc, argi, 1);
                args->render.num_frames = atoi(argv[argi+1]);
                num_frames_set = 1;
                argi += 2;
            }
            else if (arg_matches(argv[argi], "--silent", "-s")) {
                args->render.silent = 1;
                argi += 1;
            }
            else {
                print_unrecognised_argument(argv[argi]);
            }
        }

        if (!input_set) {
            print_missing_argument("--input");
        }

        if (!output_set) {
            args->render.output = replace_extension(args->render.input, "png");
        }

        if (args->render.movie && !num_frames_set) {
            print_missing_argument("--frames");
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