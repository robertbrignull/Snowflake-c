#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "algo.h"
#include "bsp.h"
#include "tga.h"

#include "image_out.h"

char *generate_image(bsp_t *b, int S) {
    char *P = (char*) malloc(S*S);
    CHECK_MEM(P);

    memset(P, 0, S*S);

    bsp_iterator it = bsp_iterator_new(b);

    while (bsp_iterator_has_next(it)) {
        bsp_result r = bsp_iterator_next(&it);

        int x = (int) floor(r.x);
        int y = (int) floor(r.y);

        if (x >= 0 && x < S && y >= 0 && y < S) {
            P[pix(x, y, S, S)] = 255;
        }
    }

    return P;
}

void find_bounds(char *P, int S, int *BN, int *BS, int *BW, int *BE) {
    for (int y = 0; y < S; y++) {
        for (int x = 0; x < S; x++) {
            if (P[pix(x, y, S, S)]) {
                *BN = y; 
                goto DoneBN;
            }
        }
    }
    DoneBN:;

    for (int y = S-1; y >= 0; y--) {
        for (int x = 0; x < S; x++) {
            if (P[pix(x, y, S, S)]) {
                *BS = y; 
                goto DoneBS;
            }
        }
    }
    DoneBS:;

    for (int x = S-1; x >= 0; x--) {
        for (int y = 0; y < S; y++) {
            if (P[pix(x, y, S, S)]) {
                *BE = x; 
                goto DoneBE;
            }
        }
    }
    DoneBE:;

    for (int x = 0; x < S; x++) {
        for (int y = 0; y < S; y++) {
            if (P[pix(x, y, S, S)]) {
                *BW = x;
                goto DoneBW;
            }
        }
    }
    DoneBW:;

    // for prettyness
    *BN -= 10;
    *BS += 10;
    *BW -= 10;
    *BE += 10;
}

char *shrink_image(char *P1, int size1, int width, int height, int left, int top) {
    char *P2 = (char*) malloc(width * height);
    CHECK_MEM(P2);

    memset(P2, 0, width * height);

    for (int x2 = 0; x2 < width; x2++) {
        for (int y2 = 0; y2 < height; y2++) {
            int x1 = x2 + left;
            int y1 = y2 + top;

            int z1 = pix(x1, y1, size1, size1);
            int z2 = pix(x2, y2, width, height);

            // do this check to handle our viewport extending
            // beyond the original image's bounds
            if (x1 >= 0 && x1 < size1 &&
                y1 >= 0 && y1 < size1 &&
                z1 >= 0 && z1 < size1*size1)
            {
                P2[z2] = P1[z1];
            }
        }
    }

    return P2;
}

void write_image(bsp_t *b, char *filename) {
    int unshrunk_size = (int) ceil(b->S);

    // Write the flake to a 2D array of pixels
    char *unshrunk_pixels = generate_image(b, unshrunk_size);

    // find the bound of the image
    int BN, BS, BW, BE;
    find_bounds(unshrunk_pixels, unshrunk_size, &BN, &BS, &BW, &BE);

    int width = BE - BW + 1;
    int height = BS - BN + 1;
    int left = BW;
    int top = BN;

    // // shrink the image to the bounds
    char *shrunk_pixels = shrink_image(unshrunk_pixels, unshrunk_size,
        width, height, left, top);

    free(unshrunk_pixels);

    write_tga(filename, shrunk_pixels, width, height);

    free(shrunk_pixels);
}
