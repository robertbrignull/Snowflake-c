#include <stdio.h>
#include <stdlib.h>

#include "algo.h"

#include "flake_log.h"

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
