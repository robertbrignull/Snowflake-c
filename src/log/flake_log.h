#pragma once

#include <stdio.h>
#include "data/flake.h"

flake *read_log_as_flake(FILE *log, int *num_particles, double *farthest_particle);

double *read_log_as_array(FILE *log, int *num_particles);

void log_new_particle(FILE *log, double x, double y);
