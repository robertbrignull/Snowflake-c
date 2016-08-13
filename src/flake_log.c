#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "algo.h"
#include "bsp.h"

#include "flake_log.h"

bsp_t *read_flake_log_as_bsp(FILE *flake, double *farthest_particle) {
    fseek(flake, 0, SEEK_SET);

    bsp_t *b = bsp_new(50);
    *farthest_particle = 0.0;

    double x, y;
    while (fscanf(flake, "%lf %lf %*d", &x, &y) == 2) {
        if (fabs(x) >= b->size || fabs(y) >= b->size) {
            b = bsp_change_size(b, b->size * 2);
        }
        bsp_add_point(b, x, y);
        *farthest_particle = fmax(*farthest_particle, dist_origin(x, y));
    }

    return b;
}

int find_number_of_particles(FILE *flake) {
    fseek(flake, 0, SEEK_SET);

    int lines = 0;
    int ch = 0;
    while ((ch = fgetc(flake)) != EOF)
    {
        if (ch == '\n') lines++;
    }
    return lines;
}

double *read_flake_log_as_array(FILE *flake, int *num_particles) {
    *num_particles = find_number_of_particles(flake);
    double *points = (double*) malloc(*num_particles * 2 * sizeof(double));
    CHECK_MEM(points);

    fseek(flake, 0, SEEK_SET);

    double x, y;
    for (int i = 0; i < *num_particles; i++) {
        if (fscanf(flake, "%lf %lf %*d", &x, &y) != 2) {
            fprintf(stderr, "Could not read line from log file\n");
        }

        points[2*i] = x;
        points[2*i+1] = y;
    }

    return points;
}

void log_new_particle(FILE *flake, double x, double y, int num_iterations) {
    fprintf(flake, "%f %f %d\n", x, y, num_iterations);
}
