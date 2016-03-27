#include <stdio.h>
#include <stdlib.h>

#include "algo.h"

#include "bsp.h"

// Declare private functions
int BSP_newNode(BSP_t *b, BSP_type_e type, int parent);
void BSP_addPointImpl(BSP_t *b, int n);
BSP_result BSP_findNearestImpl(BSP_t *b, int n);
void BSP_printImpl(BSP_t *b, int n);

// Used as arguments to most internal functions
BSP_type_e lastType;
double px, py, ox, oy, w, h;



// Creates an empty BSP tree of the given size
BSP_t *BSP_new(double S) {
    BSP_t *b = (BSP_t*) malloc(sizeof(BSP_t));
    CHECK_MEM(b);

    b->S = S;
    b->size = 1000;
    b->nodes = (BSP_node*) malloc(sizeof(BSP_node) * b->size);
    CHECK_MEM(b->nodes);

    b->nodes[0].type = BSP_EMPTY;
    b->nodes[0].parent = -1;
    b->numNodes = 1;

    return b;
}

// Destroys the tree, freeing memory
void BSP_destroy(BSP_t *b) {
    free(b->nodes);
    free(b);
}

// Increases the size of the BSP, returning a new one
BSP_t *BSP_increaseSize(BSP_t *b) {
    BSP_t *newB = BSP_new(b->S * 2.0);

    BSP_iterator it = BSP_iterator_new(b);

    double S2 = b->S / 2.0;
    while (BSP_iterator_hasNext(it)) {
        BSP_result r = BSP_iterator_next(&it);
        BSP_addPoint(newB, r.x + S2, r.y + S2);
    }

    return newB;
}

// Adds a point to the tree
void BSP_addPoint(BSP_t *b, double x, double y) {
    if (x < 0.0 || x >= b->S || y < 0.0 || y >= b->S) {
        printf("Cannot add (%f, %f), outside of BSP region.\n", x, y);
        return;
    }

    px = x;
    py = y;
    ox = 0.0;
    oy = 0.0;
    w = b->S;
    h = b->S;
    lastType = BSP_HLINE;

    BSP_addPointImpl(b, 0);
}

// Returns the distance to the nearest point in the tree
BSP_result BSP_findNearest(BSP_t *b, double x, double y) {
    px = x;
    py = y;
    ox = 0.0;
    oy = 0.0;
    w = b->S;
    h = b->S;

    return BSP_findNearestImpl(b, 0);
}

// For testing, prints a tree
void BSP_print(BSP_t *b) {
    ox = 0.0;
    oy = 0.0;
    w = b->S;
    h = b->S;

    BSP_printImpl(b, 0);
}

BSP_iterator BSP_iterator_new(BSP_t *b) {
    BSP_iterator i;
    i.b = b;

    if (i.b->nodes[0].type == BSP_EMPTY) {
        i.current = -1;
    }
    else {
        i.current = 0;
        BSP_node n = i.b->nodes[i.current];
        while (n.type == BSP_VLINE || n.type == BSP_HLINE) {
            i.current = (i.b->nodes[n.left].type != BSP_EMPTY) ? n.left : n.right;
            n = i.b->nodes[i.current];
        }
    }

    return i;
}

int BSP_iterator_hasNext(BSP_iterator i) {
    return (i.current != -1 && i.b->nodes[i.current].type == BSP_POINT);
}

BSP_result BSP_iterator_next(BSP_iterator *i) {
    if (i->current == -1) {
        printf("Trying to use invalid iterator.\n");
        exit(1);
    }

    BSP_result r;
    r.x = i->b->nodes[i->current].x;
    r.y = i->b->nodes[i->current].y;

    int last = i->current;
    BSP_node n = i->b->nodes[i->current];
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



// Private method, creates a new BSP tree node
int BSP_newNode(BSP_t *b, BSP_type_e type, int parent) {
    if (b->numNodes == b->size) {
        b->size *= 2;
        b->nodes = (BSP_node*) realloc(b->nodes, sizeof(BSP_node) * b->size);
        CHECK_MEM(b->nodes);
    }

    int i = b->numNodes;
    b->nodes[i].type = type;
    b->nodes[i].parent = parent;
    b->numNodes++;

    return i;
}

// Private method, adds a point to the tree
void BSP_addPointImpl(BSP_t *b, int n) {
    BSP_node *node = &b->nodes[n];

    if (node->type == BSP_HLINE) {
        double h1 = h;
        h /= 2;

        lastType = BSP_HLINE;

        if (py < oy + h) {
            BSP_addPointImpl(b, node->left);
        }
        else {
            double oy1 = oy;
            oy += h;
            BSP_addPointImpl(b, node->right);
            oy = oy1;
        }

        h = h1;
    }
    else if (node->type == BSP_VLINE) {
        double w1 = w;
        w /= 2;

        lastType = BSP_VLINE;

        if (px < ox + w) {
            BSP_addPointImpl(b, node->left);
        }
        else {
            double ox1 = ox;
            ox += w;
            BSP_addPointImpl(b, node->right);
            ox = ox1;
        }

        w = w1;
    }
    else if (node->type == BSP_POINT) {
        if (node->x != px || node->y != py) {
            double x1 = node->x;
            double y1 = node->y;

            node->type = (lastType == BSP_HLINE) ? BSP_VLINE : BSP_HLINE;

            // Can't use the pointer here as the memory address may change
            int l = BSP_newNode(b, BSP_EMPTY, n);
            int r = BSP_newNode(b, BSP_EMPTY, n);

            node = &b->nodes[n];
            node->left = l;
            node->right = r;

            BSP_addPointImpl(b, n);

            px = x1;
            py = y1;
            BSP_addPointImpl(b, n);
        }
    }
    else {
        node->type = BSP_POINT;
        node->x = px;
        node->y = py;
    }
}

BSP_result BSP_findNearestImpl(BSP_t *b, int n) {
    BSP_node node = b->nodes[n];

    if (node.type == BSP_VLINE) {
        double w1 = w;
        w /= 2;

        if (px < ox + w) {
            BSP_result d = BSP_findNearestImpl(b, node.left);

            if (d.d == 0.0) {
                double ox1 = ox;
                ox += w;
                d = BSP_findNearestImpl(b, node.right);
                ox = ox1;
            }
            else if (d.d > ox + w - px) {
                double ox1 = ox;
                ox += w;
                BSP_result d1 = BSP_findNearestImpl(b, node.right);
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
            BSP_result d = BSP_findNearestImpl(b, node.right);
            ox = ox1;

            if (d.d == 0.0) {
                d = BSP_findNearestImpl(b, node.left);
            }
            else if (d.d > px - ox - w) {
                BSP_result d1 = BSP_findNearestImpl(b, node.left);

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
            BSP_result d = BSP_findNearestImpl(b, node.left);

            if (d.d == 0.0) {
                double oy1 = oy;
                oy += h;
                d = BSP_findNearestImpl(b, node.right);
                oy = oy1;
            }
            else if (d.d > oy + h - py) {
                double oy1 = oy;
                oy += h;
                BSP_result d1 = BSP_findNearestImpl(b, node.right);
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
            BSP_result d = BSP_findNearestImpl(b, node.right);
            oy = oy1;

            if (d.d == 0.0) {
                d = BSP_findNearestImpl(b, node.left);
            }
            else if (d.d > py - oy - h) {
                BSP_result d1 = BSP_findNearestImpl(b, node.left);

                if (d1.d < d.d && d1.d != 0.0) {
                    d = d1;
                }
            }

            h = h1;

            return d;
        }
    }
    else if (node.type == BSP_POINT) {
        BSP_result r;
        r.x = node.x;
        r.y = node.y;
        r.d = dist_d(px, py, node.x, node.y);
        return r;
    }
    else {
        BSP_result r;
        r.x = r.y = r.d = 0.0;
        return r;
    }
}

void BSP_printImpl(BSP_t *b, int n) {
    BSP_node node = b->nodes[n];

    if (node.type == BSP_VLINE) {
        printf("VLine(%d, (%.1f, %.1f, %.1f, %.1f), ", n, ox, oy, w, h);
        
        double ox1 = ox;
        double w1 = w;
        w /= 2;

        BSP_printImpl(b, node.left);
        printf(", ");

        ox += w;
        BSP_printImpl(b, node.right);
        printf(")");

        ox = ox1;
        w = w1;
    }
    else if (node.type == BSP_HLINE) {
        printf("HLine(%d, (%.1f, %.1f, %.1f, %.1f), ", n, ox, oy, w, h);
        
        double oy1 = oy;
        double h1 = h;
        h /= 2;

        BSP_printImpl(b, node.left);
        printf(", ");

        oy += h;
        BSP_printImpl(b, node.right);
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