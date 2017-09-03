#include <stdio.h>
#include <stdlib.h>

#include "algo.h"
#include "data/flake.h"

#include "data/linear/linear.h"

// Creates an empty flake
linear_t *linear_new_flake() {
    linear_t *f = (linear_t*) malloc(sizeof(f));
    CHECK_MEM(f);

    f->nodes_size = 100;
    f->num_nodes = 0;
    f->nodes = (linear_node*) malloc(sizeof(linear_node) * f->nodes_size);
    CHECK_MEM(f->nodes);

    return f;
}

// Destroys the flake, freeing memory
void linear_destroy_flake(linear_t *f) {
    free(f->nodes);
    free(f);
}

// Adds a point to the flake
void linear_add_point_to_flake(linear_t *f, double x, double y) {
    if (f->num_nodes == f->nodes_size) {
        f->nodes_size *= 2;
        f->nodes = (linear_node*) realloc(f->nodes, sizeof(linear_node) * f->nodes_size);
        CHECK_MEM(f->nodes);
    }

    f->nodes[f->num_nodes].x = x;
    f->nodes[f->num_nodes].y = y;
    f->num_nodes++;
}

// Returns the distance to the nearest point in the flake
flake_result linear_find_nearest_in_flake(linear_t *f, double x, double y) {
    flake_result result;

    result.d = -1.0;
    result.x = 0.0;
    result.y = 0.0;

    for (int i = 0; i < f->num_nodes; i++) {
        flake_result other;
        other.x = f->nodes[i].x;
        other.y = f->nodes[i].y;
        other.d = dist(x, y, other.x, other.y);

        result = min_flake_result(result, other);
    }

    return result;
}
