#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "algo.h"
#include "bsp.h"

#include "snowflake_gen.h"

bsp_t *create_snowflake(int N, FILE *log) {
    double max_size = 50.0;

    bsp_t *b = bsp_new(max_size);

    bsp_add_point(b, 0.0, 0.0);

    if (log != 0) {
        fprintf(log, "%d %f %f %d\n", 0, 0.0, 0.0, 0);
    }

    double creation_standoff = 10.0;
    double destruction_standoff = 50.0;
    double creation_boundary = creation_standoff;
    double destruction_boundary = destruction_standoff;

    for (int n = 0; n < N; n++) {
        if (n % 10000 == 0) printf("%d\n", n);

        // spawn somewhere on the bounding circle
        double r = (double) rand();
        double x = creation_boundary * cos(r);
        double y = creation_boundary * sin(r);

        // set done to 1 once this particle collides
        int done = 0;
        int num_iterations = 0;
        while (!done) {
            ++num_iterations;

            // Find the closest particle in the flake
            bsp_result d = bsp_find_nearest(b, x, y);

            // Check if we've collided
            if (d.d <= 2.0) {
                x  = d.x + (x - d.x) * (1.0 / d.d);
                y  = d.y + (y - d.y) * (1.0 / d.d);
                done = 1;
            }
            else {
                // Randomly walk in some direction
                double r = (double) rand();
                x += d.d * cos(r);
                y += d.d * sin(r);

                // Check if we've left the area
                if (dist_origin_d(x, y) > destruction_boundary) {
                    // Use conformals maps to map (x, y) back onto the creation boundary
                    r = (double) rand();
                    double x1 = cos(r);
                    double y1 = sin(r);

                    // Use g(z) = r * (p + r * z) / (r + p * z)

                    // Translate (x, y) to around the origin
                    double px = x;
                    double py = y;

                    // Precalculate some things
                    double CB2 = creation_boundary*creation_boundary;
                    double px2 = px*px;
                    double py2 = py*py;
                    double pxy = px*py;
                    double x2 = x1*x1;
                    double y2 = y1*y1;
                    double xy = x1*y1;
                    double CBpx = creation_boundary*px;
                    double CBpy = creation_boundary*py;
                    double CBx = creation_boundary*x1;
                    double CBy = creation_boundary*y1;
                    double CBpx_2 = 2.0*CBpx;
                    double CBpy_2 = 2.0*CBpy;
                    double x2_y2 = x2-y2;
                    double px2_py2 = px2-py2;

                    double denominator = CB2 + CBpx_2*x1 - CBpy_2*y1 - 4.0*pxy*xy - px2*(x2+y2) - py2*x2_y2;

                    x = (CB2*(px+x1) + CBx*px2_py2 - 2.0*CBy*pxy + CBpx*x2_y2 - CBpy_2*xy) / denominator;
                    y = (CB2*(py+y1) + CBy*px2_py2 + 2.0*CBx*pxy + CBpy*x2_y2 + CBpx_2*xy) / denominator;
                }
            }
        } // particle has collided

        // possibly increase the BSP size
        if (x <= -max_size || x >= max_size || y <= -max_size || y >= max_size) {
            bsp_t *newB = bsp_increase_size(b);
            bsp_destroy(b);
            
            b = newB;
            max_size = b->S;
        }

        // add the new point
        bsp_add_point(b, x, y);

        // log the addition of this point
        if (log != 0) {
            fprintf(log, "%d %f %f %d\n", n, x, y, num_iterations);
        }

        // update the creation and destruction boundaries
        double dis = dist_origin_d(x, y);
        creation_boundary = max_d(creation_boundary, dis + creation_standoff);
        destruction_boundary = max_d(destruction_boundary, dis + destruction_standoff);
    }

    return b;
}
