#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>
#include <math.h>

#include "bsp.h"
#include "image_out.h"
#include "snowflake_gen.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: snowflake numParticles\n");
        exit(0);
    }

    // Limit memory usage to around 6GB
    struct rlimit lim;
    lim.rlim_cur = lim.rlim_max = 6000000000L;
    setrlimit(RLIMIT_AS, &lim);

    srand(time(0));

    int N;  // N is the max number of particles
    sscanf(argv[1], "%d", &N);

    bsp_t *b = create_snowflake(N);

    write_image(b, "out/output.tga");

    bsp_destroy(b);

    return 0;
}
