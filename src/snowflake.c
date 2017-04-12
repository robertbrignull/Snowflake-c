#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "args/arg_parsing.h"
#include "data/bsp_test.h"
#include "render/render_log.h"
#include "gen/snowflake_gen.h"

int main(int argc, char **argv) {
    // Limit memory usage to around 6GB
    struct rlimit lim;
    lim.rlim_cur = lim.rlim_max = 6000000000L;
    setrlimit(RLIMIT_AS, &lim);

    struct timeval t;
    gettimeofday(&t, 0);
    srand((unsigned int) t.tv_usec);

    arg_options *args = parse_args(argc, argv);

    if (args->mode == SNOWFLAKE_GEN) {
        FILE *log = 0;
        if (args->gen.output != 0) {
            log = fopen(args->gen.output, "a+b");
        }

        create_snowflake(args->gen.num_particles, log, args->gen.symmetry_degree, args->gen.symmetry_type, args->gen.silent);
    }
    else if (args->mode == RENDER) {
        FILE *log = fopen(args->render.input, "rb");

        render_log(log, args->render.output,
            args->render.colorize, args->render.movie, args->render.num_frames, args->render.silent);
    }
    else if (args->mode == BSP_TEST) {
        run_bsp_tests();
    }

    free_args(args);

    return 0;
}
