#include <stdio.h>
#include <stdlib.h>

#include "algo.h"

#include "bsp.h"

// Declare private functions
int bsp_new_node(bsp_t *b, bsp_type_e type, int parent);
void bsp_add_point_impl(bsp_t *b, int n);
bsp_result bsp_find_nearest_impl(bsp_t *b, int n);
void bsp_print_impl(bsp_t *b, int n);

// Used as arguments to most internal functions
bsp_type_e last_type;
double px, py, ox, oy, w, h;



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

    px = x;
    py = y;
    ox = -b->size;
    oy = -b->size;
    w = b->size * 2;
    h = b->size * 2;
    last_type = BSP_HLINE;

    bsp_add_point_impl(b, 0);
}

// Returns the distance to the nearest point in the tree
bsp_result bsp_find_nearest(bsp_t *b, double x, double y) {
    px = x;
    py = y;
    ox = -b->size;
    oy = -b->size;
    w = b->size * 2;
    h = b->size * 2;

    return bsp_find_nearest_impl(b, 0);
}

// For testing, prints a tree
void bsp_print(bsp_t *b) {
    ox = -b->size;
    oy = -b->size;
    w = b->size * 2;
    h = b->size * 2;

    bsp_print_impl(b, 0);
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
void bsp_add_point_impl(bsp_t *b, int n) {
    bsp_node *node = &b->nodes[n];

    if (node->type == BSP_HLINE) {
        double h1 = h;
        h /= 2;

        last_type = BSP_HLINE;

        if (py < oy + h) {
            bsp_add_point_impl(b, node->left);
        }
        else {
            double oy1 = oy;
            oy += h;
            bsp_add_point_impl(b, node->right);
            oy = oy1;
        }

        h = h1;
    }
    else if (node->type == BSP_VLINE) {
        double w1 = w;
        w /= 2;

        last_type = BSP_VLINE;

        if (px < ox + w) {
            bsp_add_point_impl(b, node->left);
        }
        else {
            double ox1 = ox;
            ox += w;
            bsp_add_point_impl(b, node->right);
            ox = ox1;
        }

        w = w1;
    }
    else if (node->type == BSP_POINT) {
        if (node->x != px || node->y != py) {
            double x1 = node->x;
            double y1 = node->y;

            node->type = (last_type == BSP_HLINE) ? BSP_VLINE : BSP_HLINE;

            // Can't use the pointer here as the memory address may change
            int l = bsp_new_node(b, BSP_EMPTY, n);
            int r = bsp_new_node(b, BSP_EMPTY, n);

            node = &b->nodes[n];
            node->left = l;
            node->right = r;

            bsp_add_point_impl(b, n);

            px = x1;
            py = y1;
            bsp_add_point_impl(b, n);
        }
    }
    else {
        node->type = BSP_POINT;
        node->x = px;
        node->y = py;
    }
}

bsp_result bsp_find_nearest_impl(bsp_t *b, int n) {
    bsp_node node = b->nodes[n];

    if (node.type == BSP_VLINE) {
        double w1 = w;
        w /= 2;

        if (px < ox + w) {
            bsp_result d = bsp_find_nearest_impl(b, node.left);

            if (d.d == 0.0) {
                double ox1 = ox;
                ox += w;
                d = bsp_find_nearest_impl(b, node.right);
                ox = ox1;
            }
            else if (d.d > ox + w - px) {
                double ox1 = ox;
                ox += w;
                bsp_result d1 = bsp_find_nearest_impl(b, node.right);
                ox = ox1;

                if (d1.d < d.d && d1.d != 0.0) {
                    d = d1;
                }
            }

            w = w1;

            return d;
        }
        else {
            double ox1 = ox;
            ox += w;
            bsp_result d = bsp_find_nearest_impl(b, node.right);
            ox = ox1;

            if (d.d == 0.0) {
                d = bsp_find_nearest_impl(b, node.left);
            }
            else if (d.d > px - ox - w) {
                bsp_result d1 = bsp_find_nearest_impl(b, node.left);

                if (d1.d < d.d && d1.d != 0.0) {
                    d = d1;
                }
            }

            w = w1;

            return d;
        }
    }
    else if (node.type == BSP_HLINE) {
        double h1 = h;
        h /= 2;

        if (py < oy + h) {
            bsp_result d = bsp_find_nearest_impl(b, node.left);

            if (d.d == 0.0) {
                double oy1 = oy;
                oy += h;
                d = bsp_find_nearest_impl(b, node.right);
                oy = oy1;
            }
            else if (d.d > oy + h - py) {
                double oy1 = oy;
                oy += h;
                bsp_result d1 = bsp_find_nearest_impl(b, node.right);
                oy = oy1;

                if (d1.d < d.d && d1.d != 0.0) {
                    d = d1;
                }
            }

            h = h1;

            return d;
        }
        else {
            double oy1 = oy;
            oy += h;
            bsp_result d = bsp_find_nearest_impl(b, node.right);
            oy = oy1;

            if (d.d == 0.0) {
                d = bsp_find_nearest_impl(b, node.left);
            }
            else if (d.d > py - oy - h) {
                bsp_result d1 = bsp_find_nearest_impl(b, node.left);

                if (d1.d < d.d && d1.d != 0.0) {
                    d = d1;
                }
            }

            h = h1;

            return d;
        }
    }
    else if (node.type == BSP_POINT) {
        bsp_result r;
        r.x = node.x;
        r.y = node.y;
        r.d = dist_d(px, py, node.x, node.y);
        return r;
    }
    else {
        bsp_result r;
        r.x = r.y = r.d = 0.0;
        return r;
    }
}

void bsp_print_impl(bsp_t *b, int n) {
    bsp_node node = b->nodes[n];

    if (node.type == BSP_VLINE) {
        printf("VLine(%d, (%.1f, %.1f, %.1f, %.1f), ", n, ox, oy, w, h);
        
        double ox1 = ox;
        double w1 = w;
        w /= 2;

        bsp_print_impl(b, node.left);
        printf(", ");

        ox += w;
        bsp_print_impl(b, node.right);
        printf(")");

        ox = ox1;
        w = w1;
    }
    else if (node.type == BSP_HLINE) {
        printf("HLine(%d, (%.1f, %.1f, %.1f, %.1f), ", n, ox, oy, w, h);
        
        double oy1 = oy;
        double h1 = h;
        h /= 2;

        bsp_print_impl(b, node.left);
        printf(", ");

        oy += h;
        bsp_print_impl(b, node.right);
        printf(")");

        oy = oy1;
        h = h1;
    }
    else if (node.type == BSP_POINT) {
        printf("Point(%d, (%.1f, %.1f, %.1f, %.1f), %.1f, %.1f)", n, ox, oy, w, h, node.x, node.y);
    }
    else {
        printf("Empty(%d, (%.1f, %.1f, %.1f, %.1f))", n, ox, oy, w, h);
    }
}
