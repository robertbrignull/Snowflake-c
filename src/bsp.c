#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "algo.h"

#include "bsp.h"

// Declare private functions
int bsp_new_empty_node(bsp_t *b);
void bsp_add_point_impl(bsp_t *b, int node_index, double node_x, double node_y, double node_size);
bsp_result bsp_find_nearest_impl(bsp_t *b, int node_index, double node_x, double node_y, double node_size);
void bsp_print_impl(bsp_t *b, int node_index, double node_x, double node_y, double node_size);

// global constants-ish used by bsp_add_point_impl and bsp_find_nearest_impl
double point_x, point_y;



// Creates an empty bsp tree of the given size
bsp_t *bsp_new(double S) {
    bsp_t *b = (bsp_t*) malloc(sizeof(bsp_t));
    CHECK_MEM(b);

    b->size = S;
    b->nodes_size = 1000;
    b->nodes = (bsp_node*) malloc(sizeof(bsp_node) * b->nodes_size);
    CHECK_MEM(b->nodes);

    b->buckets_size = 1000;
    b->buckets = (bsp_bucket*) malloc(sizeof(bsp_bucket) * b->buckets_size);
    CHECK_MEM(b->buckets);

    b->nodes[0].type = BSP_BUCKET;
    b->nodes[0].bucket = 0;
    b->buckets[0].size = 0;
    b->num_nodes = 1;
    b->num_buckets = 1;

    return b;
}

// Destroys the tree, freeing memory
void bsp_destroy(bsp_t *b) {
    free(b->nodes);
    free(b->buckets);
    free(b);
}

// A convenience method to create a new bsp of the given
// size, copy across all points, and destroy the old bsp.
bsp_t *bsp_change_size(bsp_t *b, double new_size) {
    bsp_t *new_b = bsp_new(new_size);

    for (int i = 0; i < b->num_nodes; i++) {
        bsp_node node = b->nodes[i];
        if (node.type == BSP_BUCKET) {
            bsp_bucket *bucket = &(b->buckets[node.bucket]);
            for (int i = 0; i < bucket->size; i++) {
                bsp_add_point(new_b, bucket->points[i].x, bucket->points[i].y);
            }
        }
    }

    bsp_destroy(b);

    return new_b;
}

// Adds a point to the tree
void bsp_add_point(bsp_t *b, double x, double y) {
    if (x <= -b->size || x >= b->size || y <= -b->size || y >= b->size) {
        printf("Cannot add (%f, %f), outside of bsp region.\n", x, y);
        return;
    }

    point_x = x;
    point_y = y;
    bsp_add_point_impl(b, 0, -b->size, -b->size, b->size * 2);
}

// Returns the distance to the nearest point in the tree
bsp_result bsp_find_nearest(bsp_t *b, double x, double y) {
    point_x = x;
    point_y = y;
    return bsp_find_nearest_impl(b, 0, -b->size, -b->size, b->size * 2);
}

// For testing, prints a tree
void bsp_print(bsp_t *b) {
    bsp_print_impl(b, 0, -b->size, -b->size, b->size * 2);
}



// Private method, creates a new bsp tree node
int bsp_new_empty_node(bsp_t *b) {
    if (b->num_nodes == b->nodes_size) {
        b->nodes_size *= 2;
        b->nodes = (bsp_node*) realloc(b->nodes, sizeof(bsp_node) * b->nodes_size);
        CHECK_MEM(b->nodes);
    }

    if (b->num_buckets == b->buckets_size) {
        b->buckets_size *= 2;
        b->buckets = (bsp_bucket*) realloc(b->buckets, sizeof(bsp_bucket) * b->buckets_size);
        CHECK_MEM(b->buckets);
    }

    int i = b->num_nodes;
    b->nodes[i].type = BSP_BUCKET;
    b->nodes[i].bucket = b->num_buckets;
    b->buckets[b->num_buckets].size = 0;
    b->num_nodes++;
    b->num_buckets++;

    return i;
}

void bsp_add_to_bucket_node(bsp_t *b, int node_index, double node_x, double node_y, double node_size) {
    bsp_node node = b->nodes[node_index];
    bsp_bucket *bucket = &(b->buckets[node.bucket]);

    if (bucket->size != BSP_BUCKET_SIZE) {
        bucket->points[bucket->size].x = point_x;
        bucket->points[bucket->size].y = point_y;
        bucket->size += 1;
    }
    else {
        bsp_point old_points[BSP_BUCKET_SIZE];
        memcpy(old_points, bucket->points, sizeof(bsp_point) * BSP_BUCKET_SIZE);
        int num_old_points = bucket->size;

        node.type = BSP_CROSS;
        node.SW = bsp_new_empty_node(b);
        node.SE = bsp_new_empty_node(b);
        node.NW = bsp_new_empty_node(b);
        node.NE = bsp_new_empty_node(b);
        b->nodes[node_index] = node;

        bsp_add_point_impl(b, node_index, node_x, node_y, node_size);

        for (int i = 0; i < num_old_points; i++) {
            point_x = old_points[i].x;
            point_y = old_points[i].y;
            bsp_add_point_impl(b, node_index, node_x, node_y, node_size);
        }
    }
}

// Private method, adds a point to the tree
void bsp_add_point_impl(bsp_t *b, int node_index, double node_x, double node_y, double node_size) {
    bsp_node node = b->nodes[node_index];

    while (node.type == BSP_CROSS) {
        int south = point_y < node_y + node_size / 2;
        int west = point_x < node_x + node_size / 2;

        if (south && west) {
            node_index = node.SW;
        }
        else if (south) {
            node_index = node.SE;
            node_x += node_size / 2;
        }
        else if (west) {
            node_index = node.NW;
            node_y += node_size / 2;
        }
        else {
            node_index = node.NE;
            node_x += node_size / 2;
            node_y += node_size / 2;
        }

        node_size = node_size / 2;
        node = b->nodes[node_index];
    }

    bsp_add_to_bucket_node(b, node_index, node_x, node_y, node_size);
}

bsp_result min_bsp_result(bsp_result r1, bsp_result r2) {
    return (r1.d == -1.0 || (r2.d != -1.0 && r2.d < r1.d)) ? r2 : r1;
}

bsp_result bsp_find_nearest_impl(bsp_t *b, int node_index, double node_x, double node_y, double node_size) {
    bsp_node node = b->nodes[node_index];

    if (node.type == BSP_CROSS) {
        double dx = point_x - node_x - node_size / 2;
        double dy = point_y - node_y - node_size / 2;
        double adx = fabs(dx);
        double ady = fabs(dy);
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

        bsp_result r = bsp_find_nearest_impl(b, N, node_x + node_size / 2 * N_x_modifier, node_y + node_size / 2 * N_y_modifier, node_size / 2);

        if (r.d != -1.0 && r.d <= adx && r.d <= ady) {
            return r;
        }

        if (adx < ady) {
            r = min_bsp_result(r, bsp_find_nearest_impl(b, H, node_x + node_size / 2 * (1 - N_x_modifier), node_y + node_size / 2 * N_y_modifier, node_size / 2));

            if (r.d != -1.0 && r.d <= ady) {
                return r;
            }

            r = min_bsp_result(r, bsp_find_nearest_impl(b, V, node_x + node_size / 2 * N_x_modifier, node_y + node_size / 2 * (1 - N_y_modifier), node_size / 2));

            if (r.d != -1.0 && r.d <= dist_origin(dx, dy)) {
                return r;
            }

            return min_bsp_result(r, bsp_find_nearest_impl(b, F, node_x + node_size / 2 * (1 - N_x_modifier), node_y + node_size / 2 * (1 - N_y_modifier), node_size / 2));
        }
        else {
            r = min_bsp_result(r, bsp_find_nearest_impl(b, V, node_x + node_size / 2 * N_x_modifier, node_y + node_size / 2 * (1 - N_y_modifier), node_size / 2));

            if (r.d != -1.0 && r.d <= adx) {
                return r;
            }

            r = min_bsp_result(r, bsp_find_nearest_impl(b, H, node_x + node_size / 2 * (1 - N_x_modifier), node_y + node_size / 2 * N_y_modifier, node_size / 2));

            if (r.d != -1.0 && r.d <= dist_origin(dx, dy)) {
                return r;
            }

            return min_bsp_result(r, bsp_find_nearest_impl(b, F, node_x + node_size / 2 * (1 - N_x_modifier), node_y + node_size / 2 * (1 - N_y_modifier), node_size / 2));
        }
    }
    else if (node.type == BSP_BUCKET) {
        bsp_bucket *bucket = &(b->buckets[node.bucket]);

        bsp_result r;
        r.x = bucket->points[0].x;
        r.y = bucket->points[0].y;
        r.d = dist(point_x, point_y, bucket->points[0].x, bucket->points[0].y);

        for (int i = 1; i < bucket->size; i++) {
            bsp_result r2;
            r2.x = bucket->points[i].x;
            r2.y = bucket->points[i].y;
            r2.d = dist(point_x, point_y, bucket->points[i].x, bucket->points[i].y);
            r = min_bsp_result(r, r2);
        }

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
    else if (node.type == BSP_BUCKET) {
        printf("Bucket(%d, (%.1f, %.1f, %.1f), [", node_index, node_x, node_y, node_size);
        bsp_bucket *bucket = &(b->buckets[node.bucket]);
        for (int i = 0; i < bucket->size; i++) {
            if (i != 0) printf(", ");
            printf("(%.1f, %.1f)", bucket->points[i].x, bucket->points[i].y);
        }
        printf("])");
    }
    else {
        printf("Empty(%d, (%.1f, %.1f, %.1f))", node_index, node_x, node_y, node_size);
    }
}
