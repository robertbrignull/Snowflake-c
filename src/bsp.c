#include <stdio.h>
#include <stdlib.h>

#include "algo.h"

#include "bsp.h"

// Declare private functions
int bsp_new_node(bsp_t *b, bsp_type_e type, int parent);
void bsp_add_point_impl(bsp_t *b, int node_index, double point_x, double point_y, double node_x, double node_y, double node_width, double node_height, bsp_type_e last_type);
bsp_result bsp_find_nearest_impl(bsp_t *b, int node_index, double point_x, double point_y, double node_x, double node_y, double node_width, double node_height);
void bsp_print_impl(bsp_t *b, int node_index, double node_x, double node_y, double node_width, double node_height);



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

    bsp_add_point_impl(b, 0, x, y, -b->size, -b->size, b->size * 2, b->size * 2, BSP_HLINE);
}

// Returns the distance to the nearest point in the tree
bsp_result bsp_find_nearest(bsp_t *b, double x, double y) {
    return bsp_find_nearest_impl(b, 0, x, y, -b->size, -b->size, b->size * 2, b->size * 2);
}

// For testing, prints a tree
void bsp_print(bsp_t *b) {
    bsp_print_impl(b, 0, -b->size, -b->size, b->size * 2, b->size * 2);
}

bsp_iterator bsp_iterator_new(bsp_t *b) {
    bsp_iterator i;
    i.b = b;

    if (i.b->nodes[0].type == BSP_EMPTY) {
        i.current = -1;
    }
    else {
        i.current = 0;
        bsp_node n = i.b->nodes[i.current];
        while (n.type == BSP_VLINE || n.type == BSP_HLINE) {
            i.current = (i.b->nodes[n.left].type != BSP_EMPTY) ? n.left : n.right;
            n = i.b->nodes[i.current];
        }
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

    int last = i->current;
    bsp_node n = i->b->nodes[i->current];
    do {
        last = i->current;
        i->current = n.parent;
        if (i->current != -1) {
            n = i->b->nodes[i->current];
        }
    } while (i->current != -1 && (n.right == last || i->b->nodes[n.right].type == BSP_EMPTY));

    if (i->current != -1) {
        i->current = n.right;
        n = i->b->nodes[i->current];

        while (n.type == BSP_VLINE || n.type == BSP_HLINE) {
            i->current = (i->b->nodes[n.left].type != BSP_EMPTY) ? n.left : n.right;
            n = i->b->nodes[i->current];
        }
    }

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
void bsp_add_point_impl(bsp_t *b, int node_index, double point_x, double point_y, double node_x, double node_y, double node_width, double node_height, bsp_type_e last_type) {
    bsp_node *node = &b->nodes[node_index];

    if (node->type == BSP_HLINE) {
        if (point_y < node_y + node_height / 2) {
            bsp_add_point_impl(b, node->left, point_x, point_y, node_x, node_y, node_width, node_height / 2, BSP_HLINE);
        }
        else {
            bsp_add_point_impl(b, node->right, point_x, point_y, node_x, node_y + node_height / 2, node_width, node_height / 2, BSP_HLINE);
        }
    }
    else if (node->type == BSP_VLINE) {
        if (point_x < node_x + node_width / 2) {
            bsp_add_point_impl(b, node->left, point_x, point_y, node_x, node_y, node_width / 2, node_height, BSP_VLINE);
        }
        else {
            bsp_add_point_impl(b, node->right, point_x, point_y, node_x + node_width / 2, node_y, node_width / 2, node_height, BSP_VLINE);
        }
    }
    else if (node->type == BSP_POINT) {
        if (node->x != point_x || node->y != point_y) {
            node->type = (last_type == BSP_HLINE) ? BSP_VLINE : BSP_HLINE;

            double old_point_x = node->x;
            double old_point_y = node->y;

            int l = bsp_new_node(b, BSP_EMPTY, node_index);
            int r = bsp_new_node(b, BSP_EMPTY, node_index);
            
            // need to re-get the pointer to node now as bsp_new_node may have changed it by resizing the array
            node = &b->nodes[node_index];

            node->left = l;
            node->right = r;

            bsp_add_point_impl(b, node_index, point_x, point_y, node_x, node_y, node_width, node_height, last_type);
            bsp_add_point_impl(b, node_index, old_point_x, old_point_y, node_x, node_y, node_width, node_height, last_type);
        }
    }
    else {
        node->type = BSP_POINT;
        node->x = point_x;
        node->y = point_y;
    }
}

bsp_result min_bsp_result(bsp_result b1, bsp_result b2) {
    return (b2.d == 0.0 || (b1.d != 0.0 && b1.d < b2.d)) ? b1 : b2;
}

bsp_result bsp_find_nearest_impl(bsp_t *b, int node_index, double point_x, double point_y, double node_x, double node_y, double node_width, double node_height) {
    bsp_node node = b->nodes[node_index];

    if (node.type == BSP_VLINE) {
        if (point_x < node_x + node_width / 2) {
            bsp_result d = bsp_find_nearest_impl(b, node.left, point_x, point_y, node_x, node_y, node_width / 2, node_height);

            if (d.d != 0.0 && d.d <= node_x + node_width / 2 - point_x) {
                return d;
            }
            
            return min_bsp_result(d, bsp_find_nearest_impl(b, node.right, point_x, point_y, node_x + node_width / 2, node_y, node_width / 2, node_height));
        }
        else {
            bsp_result d = bsp_find_nearest_impl(b, node.right, point_x, point_y, node_x + node_width / 2, node_y, node_width / 2, node_height);

            if (d.d != 0.0 && d.d <= point_x - node_x - node_width / 2) {
                return d;
            }
            
            return min_bsp_result(d, bsp_find_nearest_impl(b, node.left, point_x, point_y, node_x, node_y, node_width / 2, node_height));
        }
    }
    else if (node.type == BSP_HLINE) {
        if (point_y < node_y + node_height / 2) {
            bsp_result d = bsp_find_nearest_impl(b, node.left, point_x, point_y, node_x, node_y, node_width, node_height / 2);

            if (d.d != 0.0 && d.d <= node_y + node_height / 2 - point_y) {
                return d;
            }
            
            return min_bsp_result(d, bsp_find_nearest_impl(b, node.right, point_x, point_y, node_x, node_y + node_height / 2, node_width, node_height / 2));
        }
        else {
            bsp_result d = bsp_find_nearest_impl(b, node.right, point_x, point_y, node_x, node_y + node_height / 2, node_width, node_height / 2);

            if (d.d != 0.0 && d.d <= point_y - node_y - node_height / 2) {
                return d;
            }
            
            return min_bsp_result(d, bsp_find_nearest_impl(b, node.left, point_x, point_y, node_x, node_y, node_width, node_height / 2));
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
        r.x = r.y = r.d = 0.0;
        return r;
    }
}

void bsp_print_impl(bsp_t *b, int node_index, double node_x, double node_y, double node_width, double node_height) {
    bsp_node node = b->nodes[node_index];

    if (node.type == BSP_VLINE) {
        printf("VLine(%d, (%.1f, %.1f, %.1f, %.1f), ", node_index, node_x, node_y, node_width, node_height);
        
        bsp_print_impl(b, node.left, node_x, node_y, node_width / 2, node_height);
        printf(", ");

        bsp_print_impl(b, node.right, node_x + node_width / 2, node_y, node_width / 2, node_height);
        printf(")");
    }
    else if (node.type == BSP_HLINE) {
        printf("HLine(%d, (%.1f, %.1f, %.1f, %.1f), ", node_index, node_x, node_y, node_width, node_height);
        
        bsp_print_impl(b, node.left, node_x, node_y, node_width, node_height / 2);
        printf(", ");

        bsp_print_impl(b, node.right, node_x, node_y + node_height / 2, node_width, node_height / 2);
        printf(")");
    }
    else if (node.type == BSP_POINT) {
        printf("Point(%d, (%.1f, %.1f, %.1f, %.1f), %.1f, %.1f)", node_index, node_x, node_y, node_width, node_height, node.x, node.y);
    }
    else {
        printf("Empty(%d, (%.1f, %.1f, %.1f, %.1f))", node_index, node_x, node_y, node_width, node_height);
    }
}
