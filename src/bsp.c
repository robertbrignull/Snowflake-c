#include <stdio.h>
#include <stdlib.h>

#include "algo.h"

#include "bsp.h"

// Declare private functions
int bsp_new_node(bsp_t *b, bsp_type_e type, int parent);
void bsp_add_point_impl(bsp_t *b, int node_index, double point_x, double point_y, double node_x, double node_y, double node_size);
bsp_result bsp_find_nearest_impl(bsp_t *b, int node_index, double point_x, double point_y, double node_x, double node_y, double node_size);
void bsp_print_impl(bsp_t *b, int node_index, double node_x, double node_y, double node_size);



// Creates an empty bsp tree of the given size
bsp_t *bsp_new(double S) {
    bsp_t *b = (bsp_t*) malloc(sizeof(bsp_t));
    CHECK_MEM(b);

    b->size = S;
    b->nodes_size = 1000;
    b->nodes = (bsp_node*) malloc(sizeof(bsp_node) * b->nodes_size);
    CHECK_MEM(b->nodes);

    b->nodes[0].type = BSP_EMPTY;
    b->nodes[0].parent = -1;
    b->num_nodes = 1;

    return b;
}

// Destroys the tree, freeing memory
void bsp_destroy(bsp_t *b) {
    free(b->nodes);
    free(b);
}

// Increases the size of the bsp, returning a new one
bsp_t *bsp_increase_size(bsp_t *b) {
    bsp_t *new_b = bsp_new(b->size * 2.0);

    bsp_iterator it = bsp_iterator_new(b);
    while (bsp_iterator_has_next(it)) {
        bsp_result r = bsp_iterator_next(&it);
        bsp_add_point(new_b, r.x, r.y);
    }

    return new_b;
}

// Adds a point to the tree
void bsp_add_point(bsp_t *b, double x, double y) {
    if (x <= -b->size || x >= b->size || y <= -b->size || y >= b->size) {
        printf("Cannot add (%f, %f), outside of bsp region.\n", x, y);
        return;
    }

    bsp_add_point_impl(b, 0, x, y, -b->size, -b->size, b->size * 2);
}

// Returns the distance to the nearest point in the tree
bsp_result bsp_find_nearest(bsp_t *b, double x, double y) {
    return bsp_find_nearest_impl(b, 0, x, y, -b->size, -b->size, b->size * 2);
}

// For testing, prints a tree
void bsp_print(bsp_t *b) {
    bsp_print_impl(b, 0, -b->size, -b->size, b->size * 2);
}

void bsp_iterator_find_next(bsp_iterator *i, int last) {
    if (i->current == -1) {
        return;
    }

    bsp_node n = i->b->nodes[i->current];
    if (n.type == BSP_CROSS) {
        int this = i->current;
        if (n.NE == last) {
            i->current = n.parent;
        } else if (n.SE == last) {
            i->current = n.NE;
        } else if (n.NW == last) {
            i->current = n.SE;
        } else if (n.SW == last) {
            i->current = n.NW;
        } else {
            i->current = n.SW;
        }
        bsp_iterator_find_next(i, this);
    }
    else if (n.type == BSP_EMPTY) {
        int this = i->current;
        i->current = n.parent;
        bsp_iterator_find_next(i, this);
    }
}

bsp_iterator bsp_iterator_new(bsp_t *b) {
    bsp_iterator i;
    i.b = b;

    i.current = 0;
    bsp_node n = i.b->nodes[i.current];
    if (n.type == BSP_EMPTY) {
        i.current = -1;
    }
    else if (n.type == BSP_CROSS) {
        bsp_iterator_find_next(&i, -1);
    }

    return i;
}

int bsp_iterator_has_next(bsp_iterator i) {
    return (i.current != -1 && i.b->nodes[i.current].type == BSP_POINT);
}

bsp_result bsp_iterator_next(bsp_iterator *i) {
    if (i->current == -1) {
        printf("Trying to use invalid iterator.\n");
        exit(1);
    }

    bsp_result r;
    r.x = i->b->nodes[i->current].x;
    r.y = i->b->nodes[i->current].y;

    bsp_node n = i->b->nodes[i->current];
    int this = i->current;
    i->current = n.parent;
    bsp_iterator_find_next(i, this);

    return r;
}



// Private method, creates a new bsp tree node
int bsp_new_node(bsp_t *b, bsp_type_e type, int parent) {
    if (b->num_nodes == b->nodes_size) {
        b->nodes_size *= 2;
        b->nodes = (bsp_node*) realloc(b->nodes, sizeof(bsp_node) * b->nodes_size);
        CHECK_MEM(b->nodes);
    }

    int i = b->num_nodes;
    b->nodes[i].type = type;
    b->nodes[i].parent = parent;
    b->num_nodes++;

    return i;
}

// Private method, adds a point to the tree
void bsp_add_point_impl(bsp_t *b, int node_index, double point_x, double point_y, double node_x, double node_y, double node_size) {
    bsp_node node = b->nodes[node_index];

    if (node.type == BSP_CROSS) {
        int south = point_y < node_y + node_size / 2;
        int west = point_x < node_x + node_size / 2;

        if (south && west) {
            bsp_add_point_impl(b, node.SW, point_x, point_y, node_x, node_y, node_size / 2);
        }
        else if (south && !west) {
            bsp_add_point_impl(b, node.SE, point_x, point_y, node_x + node_size / 2, node_y, node_size / 2);
        }
        else if (!south && west) {
            bsp_add_point_impl(b, node.NW, point_x, point_y, node_x, node_y + node_size / 2, node_size / 2);
        }
        else if (!south && !west) {
            bsp_add_point_impl(b, node.NE, point_x, point_y, node_x + node_size / 2, node_y + node_size / 2, node_size / 2);
        }
    }
    else if (node.type == BSP_POINT) {
        if (node.x != point_x || node.y != point_y) {
            double old_point_x = node.x;
            double old_point_y = node.y;

            int SW = bsp_new_node(b, BSP_EMPTY, node_index);
            int SE = bsp_new_node(b, BSP_EMPTY, node_index);
            int NW = bsp_new_node(b, BSP_EMPTY, node_index);
            int NE = bsp_new_node(b, BSP_EMPTY, node_index);

            node.type = BSP_CROSS;
            node.SW = SW;
            node.SE = SE;
            node.NW = NW;
            node.NE = NE;
            b->nodes[node_index] = node;

            bsp_add_point_impl(b, node_index, point_x, point_y, node_x, node_y, node_size);
            bsp_add_point_impl(b, node_index, old_point_x, old_point_y, node_x, node_y, node_size);
        }
    }
    else {
        node.type = BSP_POINT;
        node.x = point_x;
        node.y = point_y;
        b->nodes[node_index] = node;
    }
}

bsp_result min_bsp_result(bsp_result r1, bsp_result r2) {
    return (r1.d == -1.0 || (r2.d != -1.0 && r2.d < r1.d)) ? r2 : r1;
}

bsp_result bsp_find_nearest_impl(bsp_t *b, int node_index, double point_x, double point_y, double node_x, double node_y, double node_size) {
    bsp_node node = b->nodes[node_index];

    if (node.type == BSP_CROSS) {
        double dx = point_x - node_x - node_size / 2;
        double dy = point_y - node_y - node_size / 2;
        double adx = abs_d(dx);
        double ady = abs_d(dy);
        int N_x_modifier = (dx >= 0) ? 1 : 0;
        int N_y_modifier = (dy >= 0) ? 1 : 0;

        //   N = near quadrant (the one the point is in)
        //   H = the adjacent quadrant horizontally
        //   V = the adjacent quadrant vertically
        //   F = the far quadrant that is not adjacent
        int N, H, V, F;

        if (dx < 0 && dy < 0) {
            N = node.SW;
            H = node.SE;
            V = node.NW;
            F = node.NE;
        }
        else if (dx >= 0 && dy < 0) {
            N = node.SE;
            H = node.SW;
            V = node.NE;
            F = node.NW;
        }
        else if (dx < 0 && dy >= 0) {
            N = node.NW;
            H = node.NE;
            V = node.SW;
            F = node.SE;
        }
        else { // dx >= 0 && dy >= 0
            N = node.NE;
            H = node.NW;
            V = node.SE;
            F = node.SW;
        }

        bsp_result r = bsp_find_nearest_impl(b, N, point_x, point_y, node_x + node_size / 2 * N_x_modifier, node_y + node_size / 2 * N_y_modifier, node_size / 2);

        if (r.d != -1.0 && r.d <= adx && r.d <= ady) {
            return r;
        }

        if (adx < ady) {
            r = min_bsp_result(r, bsp_find_nearest_impl(b, H, point_x, point_y, node_x + node_size / 2 * (1 - N_x_modifier), node_y + node_size / 2 * N_y_modifier, node_size / 2));

            if (r.d != -1.0 && r.d <= ady) {
                return r;
            }

            r = min_bsp_result(r, bsp_find_nearest_impl(b, V, point_x, point_y, node_x + node_size / 2 * N_x_modifier, node_y + node_size / 2 * (1 - N_y_modifier), node_size / 2));

            if (r.d != -1.0 && r.d <= dist_origin_d(dx, dy)) {
                return r;
            }

            return min_bsp_result(r, bsp_find_nearest_impl(b, F, point_x, point_y, node_x + node_size / 2 * (1 - N_x_modifier), node_y + node_size / 2 * (1 - N_y_modifier), node_size / 2));
        }
        else {
            r = min_bsp_result(r, bsp_find_nearest_impl(b, V, point_x, point_y, node_x + node_size / 2 * N_x_modifier, node_y + node_size / 2 * (1 - N_y_modifier), node_size / 2));

            if (r.d != -1.0 && r.d <= adx) {
                return r;
            }

            r = min_bsp_result(r, bsp_find_nearest_impl(b, H, point_x, point_y, node_x + node_size / 2 * (1 - N_x_modifier), node_y + node_size / 2 * N_y_modifier, node_size / 2));

            if (r.d != -1.0 && r.d <= dist_origin_d(dx, dy)) {
                return r;
            }

            return min_bsp_result(r, bsp_find_nearest_impl(b, F, point_x, point_y, node_x + node_size / 2 * (1 - N_x_modifier), node_y + node_size / 2 * (1 - N_y_modifier), node_size / 2));
        }
    }
    else if (node.type == BSP_POINT) {
        bsp_result r;
        r.x = node.x;
        r.y = node.y;
        r.d = dist_d(point_x, point_y, node.x, node.y);
        return r;
    }
    else {
        bsp_result r;
        r.x = r.y = 0.0;
        r.d = -1.0;
        return r;
    }
}

void bsp_print_impl(bsp_t *b, int node_index, double node_x, double node_y, double node_size) {
    bsp_node node = b->nodes[node_index];

    if (node.type == BSP_CROSS) {
        printf("Cross(%d, (%.1f, %.1f, %.1f), ", node_index, node_x, node_y, node_size);
        
        bsp_print_impl(b, node.SW, node_x, node_y, node_size / 2);
        printf(", ");
        
        bsp_print_impl(b, node.SE, node_x + node_size / 2, node_y, node_size / 2);
        printf(", ");
        
        bsp_print_impl(b, node.NW, node_x, node_y + node_size / 2, node_size / 2);
        printf(", ");

        bsp_print_impl(b, node.NE, node_x + node_size / 2, node_y + node_size / 2, node_size / 2);
        printf(")");
    }
    else if (node.type == BSP_POINT) {
        printf("Point(%d, (%.1f, %.1f, %.1f), %.1f, %.1f)", node_index, node_x, node_y, node_size, node.x, node.y);
    }
    else {
        printf("Empty(%d, (%.1f, %.1f, %.1f))", node_index, node_x, node_y, node_size);
    }
}
