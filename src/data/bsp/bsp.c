#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "algo.h"
#include "data/flake.h"

#include "data/bsp/bsp.h"

// Declare private functions
void bsp_add_point_impl(bsp_t *b, int node_index, double point_x, double point_y);
bsp_result bsp_find_nearest_impl(bsp_t *b, int node_index, double point_x, double point_y);



// Creates an empty bsp tree of the given size
bsp_t *new_flake(double S) {
    bsp_t *b = (bsp_t*) malloc(sizeof(bsp_t));
    CHECK_MEM(b);

    b->size = S;
    b->nodes_size = 1000;
    b->nodes = (bsp_node*) malloc(sizeof(bsp_node) * b->nodes_size);
    CHECK_MEM(b->nodes);

    b->buckets_size = 1000;
    b->buckets = (bsp_bucket*) malloc(sizeof(bsp_bucket) * b->buckets_size);
    CHECK_MEM(b->buckets);

    for (int i = 0; i < 4; i++) {
        b->nodes[0].child_types[i] = BSP_BUCKET;
        b->nodes[0].children[i] = i;
        b->buckets[i].size = 0;
    }

    b->nodes[0].node_x = - S;
    b->nodes[0].node_y = - S;
    b->nodes[0].node_size = S * 2;

    b->num_nodes = 1;
    b->num_buckets = 4;

    return b;
}

// Destroys the tree, freeing memory
void destroy_flake(bsp_t *b) {
    free(b->nodes);
    free(b->buckets);
    free(b);
}

// A convenience method to create a new bsp of the given
// size, copy across all points, and destroy the old bsp.
bsp_t *change_flake_size(bsp_t *b, double new_size) {
    bsp_t *new_b = new_flake(new_size);

    for (int i = 0; i < b->num_buckets; i++) {
        for (int j = 0; j < b->buckets[i].size; j++) {
            add_point_to_flake(new_b, b->buckets[i].points[j].x, b->buckets[i].points[j].y);
        }
    }

    destroy_flake(b);

    return new_b;
}

// Adds a point to the tree
void add_point_to_flake(bsp_t *b, double x, double y) {
    if (x <= -b->size || x >= b->size || y <= -b->size || y >= b->size) {
        printf("Cannot add (%f, %f), outside of bsp region.\n", x, y);
        return;
    }

    bsp_add_point_impl(b, 0, x, y);
}

// Returns the distance to the nearest point in the tree
bsp_result find_nearest_in_flake(bsp_t *b, double x, double y) {
    return bsp_find_nearest_impl(b, 0, x, y);
}



void bsp_add_to_bucket_node(bsp_t *b, int node_index, bsp_dir_e child, double point_x, double point_y) {
    bsp_node node = b->nodes[node_index];
    int old_bucket_index = node.children[child];
    bsp_bucket *bucket = &(b->buckets[old_bucket_index]);

    if (bucket->size != BSP_BUCKET_SIZE) {
        bucket->points[bucket->size].x = point_x;
        bucket->points[bucket->size].y = point_y;
        bucket->size += 1;
    }
    else {
        bsp_point old_points[BSP_BUCKET_SIZE];
        memcpy(old_points, bucket->points, sizeof(bsp_point) * BSP_BUCKET_SIZE);
        int num_old_points = bucket->size;

        // Ensure space for one more node
        if (b->num_nodes == b->nodes_size) {
            b->nodes_size *= 2;
            b->nodes = (bsp_node*) realloc(b->nodes, sizeof(bsp_node) * b->nodes_size);
            CHECK_MEM(b->nodes);
        }

        // Ensure space for three more buckets (we can reuse the old one)
        if (b->num_buckets + 4 > b->buckets_size) {
            b->buckets_size *= 2;
            b->buckets = (bsp_bucket*) realloc(b->buckets, sizeof(bsp_bucket) * b->buckets_size);
            CHECK_MEM(b->buckets);
        }

        int child_index = b->num_nodes;

        node.child_types[child] = BSP_CROSS;
        node.children[child] = child_index;

        bsp_node new_node;
        new_node.node_size = node.node_size / 2;
        switch (child) {
            case SW:
                new_node.node_x = node.node_x;
                new_node.node_y = node.node_y;
                break;
            case SE:
                new_node.node_x = node.node_x + new_node.node_size;
                new_node.node_y = node.node_y;
                break;
            case NW:
                new_node.node_x = node.node_x;
                new_node.node_y = node.node_y + new_node.node_size;
                break;
            case NE:
                new_node.node_x = node.node_x + new_node.node_size;
                new_node.node_y = node.node_y + new_node.node_size;
        }
        // Create three new nodes
        for (int i = 0; i < 3; i++) {
            new_node.child_types[i] = BSP_BUCKET;
            new_node.children[i] = b->num_buckets + i;
            b->buckets[b->num_buckets + i].size = 0;
        }
        // And reuse the old bucket
        new_node.child_types[3] = BSP_BUCKET;
        new_node.children[3] = old_bucket_index;
        b->buckets[old_bucket_index].size = 0;

        b->nodes[node_index] = node;
        b->nodes[child_index] = new_node;

        b->num_nodes++;
        b->num_buckets += 3;

        bsp_add_point_impl(b, child_index, point_x, point_y);

        for (int i = 0; i < num_old_points; i++) {
            point_x = old_points[i].x;
            point_y = old_points[i].y;
            bsp_add_point_impl(b, child_index, point_x, point_y);
        }
    }
}

// Private method, adds a point to the tree
void bsp_add_point_impl(bsp_t *b, int node_index, double point_x, double point_y) {
    bsp_node node;
    bsp_dir_e child;

    // Repeatedly find the quadrant the point belongs in until we reach a bucket
    while (1) {
        node = b->nodes[node_index];

        int south = point_y < node.node_y + node.node_size / 2;
        int west = point_x < node.node_x + node.node_size / 2;

        if (south && west) {
            child = SW;
        }
        else if (south) {
            child = SE;
        }
        else if (west) {
            child = NW;
        }
        else {
            child = NE;
        }

        if (node.child_types[child] == BSP_CROSS) {
            node_index = node.children[child];
        }
        else {
            break;
        }
    }

    bsp_add_to_bucket_node(b, node_index, child, point_x, point_y);
}

bsp_result min_bsp_result(bsp_result r1, bsp_result r2) {
    return (r1.d == -1.0 || (r2.d != -1.0 && r2.d < r1.d)) ? r2 : r1;
}

bsp_result bsp_find_nearest_for_child(bsp_t *b, int node_index, bsp_dir_e child, double point_x, double point_y) {
    bsp_node node = b->nodes[node_index];

    if (node.child_types[child] == BSP_CROSS) {
        return bsp_find_nearest_impl(b, node.children[child], point_x, point_y);
    }
    else {
        bsp_bucket *bucket = &(b->buckets[node.children[child]]);

        bsp_result r;

        if (bucket->size == 0) {
            r.x = r.y = 0.0;
            r.d = -1.0;
            return r;
        }

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
}

bsp_result bsp_find_nearest_impl(bsp_t *b, int node_index, double point_x, double point_y) {
    bsp_node node = b->nodes[node_index];

    double dx = point_x - node.node_x - node.node_size / 2;
    double dy = point_y - node.node_y - node.node_size / 2;
    double adx = fabs(dx);
    double ady = fabs(dy);

    // N = near quadrant (the one the point is in)
    // H = the adjacent quadrant horizontally
    // V = the adjacent quadrant vertically
    // F = the far quadrant that is not adjacent
    bsp_dir_e N, H, V, F;

    if (dx < 0 && dy < 0) {
        N = SW; H = SE; V = NW; F = NE;
    }
    else if (dx >= 0 && dy < 0) {
        N = SE; H = SW; V = NE; F = NW;
    }
    else if (dx < 0 && dy >= 0) {
        N = NW; H = NE; V = SW; F = SE;
    }
    else { // dx >= 0 && dy >= 0
        N = NE; H = NW; V = SE; F = SW;
    }

    bsp_result r = bsp_find_nearest_for_child(b, node_index, N, point_x, point_y);

    if (r.d != -1.0 && r.d <= adx && r.d <= ady) {
        return r;
    }

    if (adx < ady) {
        r = min_bsp_result(r, bsp_find_nearest_for_child(b, node_index, H, point_x, point_y));

        if (r.d != -1.0 && r.d <= ady) {
            return r;
        }

        r = min_bsp_result(r, bsp_find_nearest_for_child(b, node_index, V, point_x, point_y));

        if (r.d != -1.0 && r.d <= dist_origin(dx, dy)) {
            return r;
        }

        return min_bsp_result(r, bsp_find_nearest_for_child(b, node_index, F, point_x, point_y));
    }
    else {
        r = min_bsp_result(r, bsp_find_nearest_for_child(b, node_index, V, point_x, point_y));

        if (r.d != -1.0 && r.d <= adx) {
            return r;
        }

        r = min_bsp_result(r, bsp_find_nearest_for_child(b, node_index, H, point_x, point_y));

        if (r.d != -1.0 && r.d <= dist_origin(dx, dy)) {
            return r;
        }

        return min_bsp_result(r, bsp_find_nearest_for_child(b, node_index, F, point_x, point_y));
    }
}

