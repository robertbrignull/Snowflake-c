#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>
#include <math.h>

#include "arg_parsing.h"
#include "bsp.h"
#include "image_out.h"
#include "snowflake_gen.h"

int main(int argc, char **argv) {
    // Limit memory usage to around 6GB
    struct rlimit lim;
    lim.rlim_cur = lim.rlim_max = 6000000000L;
    setrlimit(RLIMIT_AS, &lim);

    srand(time(0));

    arg_options *args = parse_args(argc, argv);

    bsp_t *b = create_snowflake(args->num_particles);

    write_image(b, args->output);

    bsp_destroy(b);

    free_args(args);

    return 0;
}
