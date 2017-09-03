#include <stdlib.h>

#include "algo.h"
#include "data/bsp/bsp.h"

#include "data/flake.h"

// Creates an empty flake with the given size
flake *new_flake(double S) {
    flake *f = (flake*) malloc(sizeof(flake));
    CHECK_MEM(f);

    f->bsp = bsp_new_flake(S);

    return f;
}

// Destroys the flake, freeing memory
void destroy_flake(flake *f) {
    bsp_destroy_flake(f->bsp);

    free(f);
}

// Adds a point to the flake
void add_point_to_flake(flake *f, double x, double y) {
    bsp_add_point_to_flake(f->bsp, x, y);
}

// Returns the distance to the nearest point in the flake
flake_result find_nearest_in_flake(flake *f, double x, double y) {
    return bsp_find_nearest_in_flake(f->bsp, x, y);
}
