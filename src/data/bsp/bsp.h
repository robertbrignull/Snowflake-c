#pragma once

typedef enum bsp_type_e {BSP_CROSS, BSP_BUCKET} bsp_type_e;

typedef enum bsp_dir_e {SW = 0, NW = 1, SE = 2, NE = 3} bsp_dir_e;

typedef struct bsp_point {
    double x, y;
} bsp_point;

#define BSP_BUCKET_SIZE 50
typedef struct bsp_bucket {
    // A statically allocated array of points
    bsp_point points[BSP_BUCKET_SIZE];

    // How many points are in this bucket
    int size;
} bsp_bucket;

typedef struct bsp_node {
    // Whether the child is another node or a bucket
    bsp_type_e child_types[4];

    // The index of the child, either in to the nodes or the buckets arrays
    int children[4];

    double node_x, node_y, node_size;
} bsp_node;

typedef struct bsp_t {
    // The size of this bsp tree
    double size;

    // A dynamically allocated array of nodes, with the root at index 0
    bsp_node *nodes;

    // The size of the nodes array
    int nodes_size;

    // The number of nodes in the array
    int num_nodes;

    // A dynamically allocated array of buckets
    bsp_bucket *buckets;

    // The size of the buckets array
    int buckets_size;

    // The number of buckets in the array
    int num_buckets;
} bsp_t;

typedef struct bsp_result {
    // How far to the node
    double d;

    // Where actually was the node
    double x, y;
} bsp_result;



// Creates an empty bsp tree with the given size
bsp_t *new_flake(double S);

// Destroys the tree, freeing memory
void destroy_flake(bsp_t *b);

// A convenience method to create a new bsp of the given
// size, copy across all points, and destroy the old bsp.
bsp_t *change_flake_size(bsp_t *b, double new_size);

// Adds a point to the tree
void add_point_to_flake(bsp_t *b, double x, double y);

// Returns the distance to the nearest point in the tree
bsp_result find_nearest_in_flake(bsp_t *b, double x, double y);
