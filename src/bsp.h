#pragma once

typedef enum bsp_type_e {BSP_CROSS, BSP_POINT, BSP_EMPTY} bsp_type_e;

typedef struct bsp_node {
    // The type of this node, determines how to use the union below
    bsp_type_e type;

    // The parent of this node
    int parent;

    union {
        // If this node is a point then this contains
        // the coordinates of that point
        struct {
            double x, y;
        };

        // If this node is a cross then this contains
        // the indexes of its children
        struct {
            int SW, NW, SE, NE;
        };
    };
} bsp_node;

typedef struct bsp_t {
    // The size of this bsp tree
    double size;

    // A dynamically allocated array of nodes, with the root at index 1
    bsp_node *nodes;

    // The size of the nodes array
    int nodes_size;

    // The number of nodes in the array
    int num_nodes;
} bsp_t;

typedef struct bsp_result {
    // How far to the node
    double d;

    // Where actually was the node
    double x, y;
} bsp_result;



// Creates an empty bsp tree with the given size
bsp_t *bsp_new(double S);

// Destroys the tree, freeing memory
void bsp_destroy(bsp_t *b);

// A convenience method to create a new bsp of the given
// size, copy across all points, and destroy the old bsp.
bsp_t *bsp_change_size(bsp_t *b, int new_size);

// Adds a point to the tree
void bsp_add_point(bsp_t *b, double x, double y);

// Returns the distance to the nearest point in the tree
bsp_result bsp_find_nearest(bsp_t *b, double x, double y);

// For testing, prints a bsp tree
void bsp_print(bsp_t *b);
