#pragma once

#include "data/flake.h"

typedef struct linear_node {
    double x, y;
} linear_node;

typedef struct linear_t {
    // All nodes
    linear_node *nodes;
    // The size of the nodes array
    int nodes_size;
    // How many nodes are actually in the array
    int num_nodes;
} linear_t;

// Creates an empty flake
linear_t *linear_new_flake();

// Destroys the flake, freeing memory
void linear_destroy_flake(linear_t *f);

// Adds a point to the flake
void linear_add_point_to_flake(linear_t *f, double x, double y);

// Returns the distance to the nearest point in the flake
flake_result linear_find_nearest_in_flake(linear_t *f, double x, double y);
