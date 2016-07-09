#pragma once

typedef enum BSP_type_e {BSP_HLINE, BSP_VLINE, BSP_POINT, BSP_EMPTY} BSP_type_e;

typedef struct BSP_node {
    // The type of this node, determines how to use the union below
    BSP_type_e type;

    // The parent of this node
    int parent;

    union {
        // If this node is a point then this contains
        // the coordinates of that point
        struct {
            double x, y;
        };

        // If this node is a line then this contains
        // the indexes of its children
        struct {
            int left, right;
        };
    };
} BSP_node;

typedef struct BSP_t {
    // The size of this BSP tree
    double S;

    // A dynamically allocated array of nodes, with the root at index 1
    BSP_node *nodes;

    // The size of the nodes array
    int size;

    // The number of nodes in the array
    int numNodes;
} BSP_t;

typedef struct BSP_result {
    // How far to the node
    double d;

    // Where actually was the node
    double x, y;
} BSP_result;

typedef struct BSP_iterator {
    // The BSP tree we're operating on
    BSP_t *b;

    // Which node are we on currently
    int current;
} BSP_iterator;



// Creates an empty BSP tree with the given size
BSP_t *BSP_new(double S);

// Destroys the tree, freeing memory
void BSP_destroy(BSP_t *b);

// Increases the size of the BSP, returning a new one
BSP_t *BSP_increaseSize(BSP_t *b);

// Adds a point to the tree
void BSP_addPoint(BSP_t *b, double x, double y);

// Returns the distance to the nearest point in the tree
BSP_result BSP_findNearest(BSP_t *b, double x, double y);

// For testing, prints a BSP tree
void BSP_print(BSP_t *b);

// Allows you to iterate through a BSP tree
BSP_iterator BSP_iterator_new(BSP_t *b);
int BSP_iterator_hasNext(BSP_iterator i);
BSP_result BSP_iterator_next(BSP_iterator *i);
