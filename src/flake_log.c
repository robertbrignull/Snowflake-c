#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "algo.h"
#include "bsp.h"

#include "flake_log.h"

typedef struct flake_log_line {
    double x;
    double y;
} flake_log_line;

void log_new_particle(FILE *flake, double x, double y) {
    flake_log_line line;
    line.x = x;
    line.y = y;
    fwrite(&line, sizeof(flake_log_line), 1, flake);
}

/**
 * @return did the operation succeed
 */
int read_line_from_log(FILE *flake, flake_log_line *line) {
    return fread(line, sizeof(flake_log_line), 1, flake) == 1;
}

bsp_t *read_flake_log_as_bsp(FILE *flake, int *num_particles, double *farthest_particle) {
    fseek(flake, 0, SEEK_SET);

    bsp_t *b = bsp_new(50);

    *num_particles = 0;
    *farthest_particle = 0.0;

    flake_log_line line;
    while (read_line_from_log(flake, &line)) {
        if (fabs(line.x) >= b->size || fabs(line.y) >= b->size) {
            b = bsp_change_size(b, b->size * 2);
        }
        bsp_add_point(b, line.x, line.y);
        *num_particles += 1;
        *farthest_particle = fmax(*farthest_particle, dist_origin(line.x, line.y));
    }

    return b;
}

int find_number_of_particles(FILE *flake) {
    fseek(flake, 0, SEEK_END);
    int size = (int) ftell(flake);
    fseek(flake, 0, SEEK_SET);
    return size / sizeof(flake_log_line);
}

double *read_flake_log_as_array(FILE *flake, int *num_particles) {
    *num_particles = find_number_of_particles(flake);
    double *points = (double*) malloc(*num_particles * 2 * sizeof(double));
    CHECK_MEM(points);

    fseek(flake, 0, SEEK_SET);

    flake_log_line line;
    for (int i = 0; i < *num_particles; i++) {
        if (!read_line_from_log(flake, &line)) {
            fprintf(stderr, "Could not read line %d from log file\n", i);
            free(points);
            return NULL;
        }

        points[2*i] = line.x;
        points[2*i+1] = line.y;
    }

    return points;
}
