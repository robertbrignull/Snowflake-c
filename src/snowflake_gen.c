#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "algo.h"
#include "bsp.h"

#include "snowflake_gen.h"

bsp_t *create_snowflake(int N, FILE *log) {
    // S - size
    double S = 50.0;

    bsp_t *b = bsp_new(S);

    bsp_add_point(b, 0.0, 0.0);

    if (log != 0) {
        fprintf(log, "%d %f %f %d\n", 0, 0.0, 0.0, 0);
    }

    // CB - shorthand for Creation Boundary
    // DB - shorthand for Destruction Boundary
    double CB_size = 10.0;
    double DB_size = 50.0;
    double CB = CB_size;
    double DB = DB_size;

    for (int n = 0; n < N; n++) {
        if (n % 10000 == 0) printf("%d\n", n);

        // spawn somewhere on the bounding circle
        double r = (double) rand();
        double x = CB * cos(r);
        double y = CB * sin(r);

        // set c to 1 once this particle collides
        int c = 0;
        int num_iterations = 0;
        while (c == 0) {
            ++num_iterations;

            // Find the closest particle in the flake
            bsp_result d = bsp_find_nearest(b, x, y);

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
                if (dist_d(x, y, 0.0, 0.0) > DB) {
                    // Use conformals maps to map (x, y) back onto the CB
                    r = (double) rand();
                    double x1 = cos(r);
                    double y1 = sin(r);

                    // Use g(z) = r * (p + r * z) / (r + p * z)

                    // Translate (x, y) to around the origin
                    double px = x;
                    double py = y;

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
                    x = x / denom;

                    y = CB2*(py+y1) + CBy*px2_py2 + 2.0*CBx*pxy + CBpy*x2_y2 + CBpx_2*xy;
                    y = y / denom;
                }
            }
        } // particle has collided

        // possibly increase the BSP size
        if (x <= -S || x >= S || y <= -S || y >= S) {
            bsp_t *newB = bsp_increase_size(b);
            bsp_destroy(b);
            
            b = newB;
            S = b->S;
        }

        // add the new point
        bsp_add_point(b, x, y);

        // log the addition of this point
        if (log != 0) {
            fprintf(log, "%d %f %f %d\n", n, x, y, num_iterations);
        }

        // update the creation and destruction boundaries
        double dis = dist_d(x, y, 0.0, 0.0);
        CB = max_d(CB, dis + CB_size);
        DB = max_d(DB, dis + DB_size);
    }

    return b;
}
