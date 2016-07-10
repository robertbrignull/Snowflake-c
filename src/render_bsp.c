#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "algo.h"
#include "bsp.h"
#include "tga.h"

#include "render_bsp.h"

void find_bounds_bsp(bsp_t *b, int *BN, int *BS, int *BW, int *BE) {
    int M = ((int) ceil(b->S)) / 2;
    int bn = M;
    int bs = M;
    int bw = M;
    int be = M;

    bsp_iterator it = bsp_iterator_new(b);

    while (bsp_iterator_has_next(it)) {
        bsp_result r = bsp_iterator_next(&it);

        int x = (int) floor(r.x);
        int y = (int) floor(r.y);

        bn = min_i(bn, y);
        bs = max_i(bs, y);
        bw = min_i(bw, x);
        be = max_i(be, x);
    }

    *BN = bn;
    *BS = bs;
    *BW = bw;
    *BE = be;
}

char *generate_image_bsp(bsp_t *b, int *w, int *h) {
    int BN, BS, BW, BE;
    find_bounds_bsp(b, &BN, &BS, &BW, &BE);

    int width = BE - BW + 1;
    int height = BS - BN + 1;
    int left = BW;
    int top = BN;

    char *P = (char*) malloc(width * height);
    CHECK_MEM(P);

    memset(P, 0, width * height);

    bsp_iterator it = bsp_iterator_new(b);

    while (bsp_iterator_has_next(it)) {
        bsp_result r = bsp_iterator_next(&it);

        int x = ((int) floor(r.x)) - left;
        int y = ((int) floor(r.y)) - top;

        if (x >= 0 && x < width && y >= 0 && y < height) {
            P[pix(x, y, width, height)] = 255;
        }
    }

    *w = width;
    *h = height;

    return P;
}

void render_bsp(bsp_t *b, char *filename) {
    // Write the flake to a 2D array of pixels
    int width, height;
    char *P = generate_image_bsp(b, &width, &height);

    write_tga(filename, P, width, height);

    free(P);
}
