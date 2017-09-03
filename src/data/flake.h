#pragma once

#include "data/bsp/bsp.h"

typedef struct flake_result {
    // How far to the node
    double d;

    // Where actually was the node
    double x, y;
} flake_result;

// Given two flake_results determine which has the smaller d value
inline flake_result min_flake_result(flake_result r1, flake_result r2) {
    return (r1.d == -1.0 || (r2.d != -1.0 && r2.d < r1.d)) ? r2 : r1;
}



// Creates an empty flake with the given size
flake *new_flake(double S);

// Destroys the flake, freeing memory
void destroy_flake(flake *f);

// Adds a point to the flake
void add_point_to_flake(flake *f, double x, double y);

// Returns the distance to the nearest point in the flake
flake_result find_nearest_in_flake(flake *fb, double x, double y);
