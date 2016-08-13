#pragma once

#include <stdio.h>
#include "bsp.h"

double *read_flake_log_as_array(FILE *flake, int *num_particles);

void log_new_particle(FILE *flake, double x, double y, int num_iterations);
