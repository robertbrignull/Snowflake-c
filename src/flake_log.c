#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "algo.h"
#include "bsp.h"

#include "flake_log.h"

void log_new_particle(FILE *flake, double x, double y, char num_iterations) {
    fwrite(&x, sizeof(double), 1, flake);
    fwrite(&y, sizeof(double), 1, flake);
    fwrite(&num_iterations, sizeof(char), 1, flake);
}

/**
 * @return did the operation succeed
 */
int read_line_from_log(FILE *flake, double *x, double *y, char *num_iterations) {
    return fread(x, sizeof(double), 1, flake) == 1 &&
            fread(y, sizeof(double), 1, flake) == 1 &&
            fread(num_iterations, sizeof(char), 1, flake) == 1;
}

bsp_t *read_flake_log_as_bsp(FILE *flake, int *num_particles, double *farthest_particle) {
    fseek(flake, 0, SEEK_SET);

    bsp_t *b = bsp_new(50);

    *num_particles = 0;
    *farthest_particle = 0.0;

    double x, y;
    char num_iterations;
    while (read_line_from_log(flake, &x, &y, &num_iterations)) {
        if (fabs(x) >= b->size || fabs(y) >= b->size) {
            b = bsp_change_size(b, b->size * 2);
        }
        bsp_add_point(b, x, y);
        *num_particles += 1;
        *farthest_particle = fmax(*farthest_particle, dist_origin(x, y));
    }

    return b;
}

int find_number_of_particles(FILE *flake) {
    fseek(flake, 0, SEEK_END);
    int size = (int) ftell(flake);
    fseek(flake, 0, SEEK_SET);
    return size / (sizeof(double) * 2 + sizeof(char));
}

double *read_flake_log_as_array(FILE *flake, int *num_particles) {
    *num_particles = find_number_of_particles(flake);
    double *points = (double*) malloc(*num_particles * 2 * sizeof(double));
    CHECK_MEM(points);

    fseek(flake, 0, SEEK_SET);

    double x, y;
    char num_iterations;
    for (int i = 0; i < *num_particles; i++) {
        if (!read_line_from_log(flake, &x, &y, &num_iterations)) {
            fprintf(stderr, "Could not read line %d from log file\n", i);
            free(points);
            return NULL;
        }

        points[2*i] = x;
        points[2*i+1] = y;
    }

    return points;
}
