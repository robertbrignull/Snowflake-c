#include <stdlib.h>

#include "algo.h"
#include "args/arg_parsing.h"
#include "data/bsp/bsp.h"
#include "data/linear/linear.h"

// Creates an empty flake with the given size
flake *new_flake(double S, flake_impl impl) {
    flake *f = (flake*) malloc(sizeof(flake));
    CHECK_MEM(f);

    if (impl == LINEAR) {
        f->linear = linear_new_flake();
    } else {
        f->bsp = bsp_new_flake(S);
    }
    f->impl = impl;

    return f;
}

// Destroys the flake, freeing memory
void destroy_flake(flake *f) {
    if (f->impl == LINEAR) {
        linear_destroy_flake(f->linear);
    } else {
        bsp_destroy_flake(f->bsp);
    }

    free(f);
}

// Adds a point to the flake
void add_point_to_flake(flake *f, double x, double y) {
    if (f->impl == LINEAR) {
        linear_add_point_to_flake(f->linear, x, y);
    } else {
        bsp_add_point_to_flake(f->bsp, x, y);
    }
}

// Returns the distance to the nearest point in the flake
flake_result find_nearest_in_flake(flake *f, double x, double y) {
    if (f->impl == LINEAR) {
        return linear_find_nearest_in_flake(f->linear, x, y);
    } else {
        return bsp_find_nearest_in_flake(f->bsp, x, y);
    }
}
