#pragma once

#include <stdio.h>
#include "data/flake.h"

bsp_t *read_log_as_flake(FILE *flake, int *num_particles, double *farthest_particle);

double *read_log_as_array(FILE *flake, int *num_particles);

void log_new_particle(FILE *flake, double x, double y);
