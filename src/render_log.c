#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "algo.h"
#include "tga.h"

#include "render_log.h"

void find_bounds_log(FILE *log, int *BN, int *BS, int *BW, int *BE) {
    int bn = 0;
    int bs = 0;
    int bw = 0;
    int be = 0;

    fseek(log, 0, SEEK_SET);

    double d_x, d_y;
    while (fscanf(log, "%*d %lf %lf %*d", &d_x, &d_y) == 2) {
        int x = (int) floor(d_x);
        int y = (int) floor(d_y);

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

char *generate_image_log(FILE *log, int *w, int *h) {
    int BN, BS, BW, BE;
    find_bounds_log(log, &BN, &BS, &BW, &BE);

    int width = BE - BW + 1;
    int height = BS - BN + 1;
    int left = BW;
    int top = BN;

    char *P = (char*) malloc(width * height);
    CHECK_MEM(P);

    memset(P, 0, width * height);

    fseek(log, 0, SEEK_SET);

    double d_x, d_y;
    while (fscanf(log, "%*d %lf %lf %*d", &d_x, &d_y) == 2) {
        int x = ((int) floor(d_x)) - left;
        int y = ((int) floor(d_y)) - top;

        if (x >= 0 && x < width && y >= 0 && y < height) {
            P[pix(x, y, width, height)] = 255;
        }
    }

    *w = width;
    *h = height;

    return P;
}

void render_log(FILE *log, char *filename) {
    // Write the flake to a 2D array of pixels
    int width, height;
    char *P = generate_image_log(log, &width, &height);

    write_tga(filename, P, width, height);

    free(P);
}
