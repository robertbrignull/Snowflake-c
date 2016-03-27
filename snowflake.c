#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>
#include <math.h>

#include "bsp.h"
#include "algo.h"
#include "imageOut.h"

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

    double S = 100.0;
    double M = S / 2;

    BSP_t *b = BSP_new(S);

    BSP_addPoint(b, M, M);

    double CBSize = 10.0;
    double DBSize = 50.0;
    double CB = CBSize;
    double DB = DBSize;

    for (int n = 0; n < N; n++) {
        if (n % 10000 == 0) printf("%d\n", n);

        // spawn somewhere on the bounding circle
        double r = (double) rand();
        double x = M + CB * cos(r);
        double y = M + CB * sin(r);

        // set c to 1 once this particle collides
        int c = 0;
        while (c == 0) {
            // Find the closest particle in the flake
            BSP_result d = BSP_findNearest(b, x, y);

            // Check if we've collided
            if (d.d <= 2.0) {
                x  = d.x + (x - d.x) * (1.0 / d.d);
                y  = d.y + (y - d.y) * (1.0 / d.d);
                c = 1;
            }
            else {
                // Randomly walk in some direction
                double r = (double) rand();
                x += d.d * cos(r);
                y += d.d * sin(r);

                // Check if we've left the area
                if (dist_d(x, y, M, M) > DB) {
                    // Use conformals maps to map (x, y) back onto the CB
                    r = (double) rand();
                    double x1 = cos(r);
                    double y1 = sin(r);

                    // Use g(z) = r * (p + r * z) / (r + p * z)

                    // Translate (x, y) to around the origin
                    double px = x - M;
                    double py = y - M;

                    // Precalculate some things
                    double CB2 = CB*CB;
                    double px2 = px*px;
                    double py2 = py*py;
                    double pxy = px*py;
                    double x2 = x1*x1;
                    double y2 = y1*y1;
                    double xy = x1*y1;
                    double CBpx = CB*px;
                    double CBpy = CB*py;
                    double CBx = CB*x1;
                    double CBy = CB*y1;
                    double CBpx_2 = 2.0*CBpx;
                    double CBpy_2 = 2.0*CBpy;
                    double x2_y2 = x2-y2;
                    double px2_py2 = px2-py2;

                    // Calculate the denominator
                    double denom = CB2 + CBpx_2*x1 - CBpy_2*y1 - 4.0*pxy*xy - px2*(x2+y2) - py2*x2_y2;

                    x = CB2*(px+x1) + CBx*px2_py2 - 2.0*CBy*pxy + CBpx*x2_y2 - CBpy_2*xy;
                    x = M + x / denom;

                    y = CB2*(py+y1) + CBy*px2_py2 + 2.0*CBx*pxy + CBpy*x2_y2 + CBpx_2*xy;
                    y = M + y / denom;
                }
            }
        }

        if (x < 0.0 || x >= S || y < 0.0 || y >= S) {
            BSP_t *newB = BSP_increaseSize(b);
            BSP_destroy(b);

            x += M;
            y += M;
            
            b = newB;
            S = b->S;
            M = S / 2;
        }

        BSP_addPoint(b, x, y);

        double dis = dist_d(x, y, M, M);
        CB = max_d(CB, dis + CBSize);
        DB = max_d(DB, dis + DBSize);
    }

    writeImage(b, "output.tga");

    BSP_destroy(b);

    return 0;
}