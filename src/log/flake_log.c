#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "algo.h"
#include "data/flake.h"

#include "log/flake_log.h"

typedef struct flake_log_line {
    double x;
    double y;
} flake_log_line;

void log_new_particle(FILE *log, double x, double y) {
    flake_log_line line;
    line.x = x;
    line.y = y;
    fwrite(&line, sizeof(flake_log_line), 1, log);
}

/**
 * @return did the operation succeed
 */
int read_line_from_log(FILE *flake, flake_log_line *line) {
    return fread(line, sizeof(flake_log_line), 1, flake) == 1;
}

flake *read_log_as_flake(FILE *log, flake_impl impl, int *num_particles, double *farthest_particle) {
    fseek(log, 0, SEEK_SET);

    flake *f = new_flake(50, impl);

    *num_particles = 0;
    *farthest_particle = 0.0;

    flake_log_line line;
    while (read_line_from_log(log, &line)) {
        add_point_to_flake(f, line.x, line.y);
        *num_particles += 1;
        *farthest_particle = fmax(*farthest_particle, dist_origin(line.x, line.y));
    }

    return f;
}

int find_number_of_particles(FILE *log) {
    fseek(log, 0, SEEK_END);
    int size = (int) ftell(log);
    fseek(log, 0, SEEK_SET);
    return size / sizeof(flake_log_line);
}

double *read_log_as_array(FILE *log, int *num_particles) {
    *num_particles = find_number_of_particles(log);
    double *points = (double*) malloc(*num_particles * 2 * sizeof(double));
    CHECK_MEM(points);

    fseek(log, 0, SEEK_SET);

    flake_log_line line;
    for (int i = 0; i < *num_particles; i++) {
        if (!read_line_from_log(log, &line)) {
            fprintf(stderr, "Could not read line %d from log file\n", i);
            free(points);
            return NULL;
        }

        points[2*i] = line.x;
        points[2*i+1] = line.y;
    }

    return points;
}
