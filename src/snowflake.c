#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>
#include <math.h>

#include "arg_parsing.h"
#include "bsp.h"
#include "bsp_test.h"
#include "image_out.h"
#include "snowflake_gen.h"

int main(int argc, char **argv) {
    // Limit memory usage to around 6GB
    struct rlimit lim;
    lim.rlim_cur = lim.rlim_max = 6000000000L;
    setrlimit(RLIMIT_AS, &lim);

    srand(time(0));

    arg_options *args = parse_args(argc, argv);

    if (args->mode == SNOWFLAKE_GEN) {
        FILE *log = 0;
        if (args->log_output != 0) {
            log = fopen(args->log_output, "w");
        }

        bsp_t *b = create_snowflake(args->num_particles, log);

        write_image(b, args->image_output);

        bsp_destroy(b);
    }
    else if (args->mode == BSP_TEST) {
        run_bsp_tests();
    }

    free_args(args);

    return 0;
}
