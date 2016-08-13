#pragma once

#include <stdio.h>
#include "bsp.h"

bsp_t *read_flake_log_as_bsp(FILE *flake, int *num_particles, double *farthest_particle);

double *read_flake_log_as_array(FILE *flake, int *num_particles);

void log_new_particle(FILE *flake, double x, double y, int num_iterations);
