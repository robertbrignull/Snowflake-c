#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/resource.h>

#include "arg_parsing.h"
#include "bsp_test.h"
#include "render_log.h"
#include "snowflake_gen.h"

int main(int argc, char **argv) {
    // Limit memory usage to around 6GB
    struct rlimit lim;
    lim.rlim_cur = lim.rlim_max = 6000000000L;
    setrlimit(RLIMIT_AS, &lim);

    srand((unsigned int) time(0));

    arg_options *args = parse_args(argc, argv);

    if (args->mode == SNOWFLAKE_GEN) {
        FILE *log = 0;
        if (args->gen.output != 0) {
            log = fopen(args->gen.output, "a+");
        }

        create_snowflake(args->gen.num_particles, log);
    }
    else if (args->mode == RENDER) {
        FILE *log = fopen(args->render.input, "r");

        render_log(log, args->render.output,
            args->render.colorize, args->render.movie, args->render.num_frames);
    }
    else if (args->mode == BSP_TEST) {
        run_bsp_tests();
    }

    free_args(args);

    return 0;
}
