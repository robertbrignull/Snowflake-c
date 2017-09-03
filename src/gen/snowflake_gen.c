#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <zconf.h>

#include "algo.h"
#include "data/flake.h"
#include "log/flake_log.h"

#include "gen/snowflake_gen.h"

void add_point(flake *b, FILE *log, double x, double y) {
    // add the new point
    add_point_to_flake(b, x, y);

    // log the addition of this point
    log_new_particle(log, x, y);
}

void create_snowflake(int N, FILE *log, int symmetry_degree, symmetry_type_enum symmetry_type, int silent) {
    int num_particles = 0;
    double farthest_particle = 0.0;
    flake *b = read_log_as_flake(log, &num_particles, &farthest_particle);

    double creation_standoff = 10.0;
    double destruction_standoff = 50.0;
    double creation_boundary = creation_standoff;
    double destruction_boundary = destruction_standoff;

    if (num_particles == 0) {
        num_particles = 1;
        N -= 1;
        add_point_to_flake(b, 0.0, 0.0);
        log_new_particle(log, 0.0, 0.0);
    }
    else {
        creation_boundary += farthest_particle;
        destruction_boundary += farthest_particle;
    }

    // Set up stdin for non-blocking IO
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    if (!silent) {
        printf("Press enter to stop...\n");
    }

    int n = num_particles;
    while (N < 0 || n < num_particles + N) {
        // Check if we should stop
        if (getc(stdin) != -1) {
            break;
        }

        // spawn somewhere on the bounding circle
        double spawn_dir = (double) rand();
        double x = creation_boundary * cos(spawn_dir);
        double y = creation_boundary * sin(spawn_dir);

        // set done to 1 once this particle collides
        int done = 0;
        char num_iterations = 0;
        while (!done) {
            num_iterations += 1;

            // Find the closest particle in the flake
            flake_result d = find_nearest_in_flake(b, x, y);

            // Check if we've collided
            if (d.d <= 2.0) {
                x  = d.x + (x - d.x) * (1.0 / d.d);
                y  = d.y + (y - d.y) * (1.0 / d.d);
                done = 1;
            }
            else {
                // Randomly walk in some direction
                double walk_dir = (double) rand();
                x += d.d * cos(walk_dir);
                y += d.d * sin(walk_dir);

                // Check if we've collided without another iteration
                if (dist(x, y, d.x, d.y) <= 2.0) {
                    num_iterations += 1;
                    done = 1;
                }
                // Check if we've left the area
                else if (dist_origin(x, y) > destruction_boundary) {
                    // Use conformal maps to map (x, y) back onto the creation boundary
                    double map_dir = (double) rand();
                    double x1 = cos(map_dir);
                    double y1 = sin(map_dir);

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

        double dis = dist_origin(x, y);

        if (symmetry_type == NONE) {
            add_point(b, log, x, y);

            // update the console progress
            n += 1;
        }
        else {
            double r = atan2(y, x);
            double rotation_per_degree = 2.0 * M_PI / symmetry_degree;
            for (int degree = 0; degree < symmetry_degree; degree++) {
                double xp = dis * cos(r);
                double yp = dis * sin(r);
                add_point(b, log, xp, yp);
                if (symmetry_type == FULL) {
                    add_point(b, log, -xp, yp);
                }
                r += rotation_per_degree;
            }

            // update the console progress
            n += symmetry_degree * ((symmetry_type == FULL) ? 2 : 1);
        }

        if (!silent) {
            printf("\r%d particles", n);
        }

        // update the creation and destruction boundaries
        creation_boundary = fmax(creation_boundary, dis + creation_standoff);
        destruction_boundary = fmax(destruction_boundary, dis + destruction_standoff);
    }

    destroy_flake(b);
}
