#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "bsp.h"
#include "algo.h"

char *generateImage(BSP_t *b, int S) {
    char *P = (char*) malloc(S*S);
    CHECK_MEM(P);

    memset(P, 0, S*S);

    BSP_iterator it = BSP_iterator_new(b);

    while (BSP_iterator_hasNext(it)) {
        BSP_result r = BSP_iterator_next(&it);

        int x = (int) floor(r.x);
        int y = (int) floor(r.y);

        P[y*S + x] = 255;
    }

    return P;
}

void writeRun(FILE *out, int v, int l) {
    unsigned char header = 127 + l;
    unsigned char data = v;

    fwrite(&header, 1, 1, out);
    fwrite(&data, 1, 1, out);
}

void writeImage(BSP_t *b, char *filename) {
    int S = (int) ceil(b->S);

    // Write the flake to a 2D array of pixels
    char *P = generateImage(b, S);

    // Find the bounds of the image
    int BN, BS, BW, BE;

    for (int y = 0; y < S; y++) {
        for (int x = 0; x < S; x++) {
            if (P[y*S+x]) {
                BN = y; 
                goto DoneBN;
            }
        }
    }
    DoneBN:;

    for (int y = S-1; y >= 0; y--) {
        for (int x = 0; x < S; x++) {
            if (P[y*S+x]) {
                BS = y; 
                goto DoneBS;
            }
        }
    }
    DoneBS:;

    for (int x = S-1; x >= 0; x--) {
        for (int y = 0; y < S; y++) {
            if (P[y*S+x]) {
                BE = x; 
                goto DoneBE;
            }
        }
    }
    DoneBE:;

    for (int x = 0; x < S; x++) {
        for (int y = 0; y < S; y++) {
            if (P[y*S+x]) {
                BW = x;
                goto DoneBW;
            }
        }
    }
    DoneBW:;

    // for prettyness
    BN -= 10;
    BS += 10;
    BW -= 10;
    BE += 10;



    FILE *out = fopen(filename, "w");

    unsigned char IDLength = 0;
    unsigned char cMapType = 0;
    unsigned char imageType = 11;

    unsigned short cMapIndex = 0;
    unsigned short cMapLength = 0;
    unsigned char cMapS = 0;

    unsigned short xOrigin = 0;
    unsigned short yOrigin = 0;
    unsigned short width = BE - BW + 1;
    unsigned short height = BS - BN + 1;
    unsigned char depth = 8;
    unsigned char descriptor = 0;



    fwrite(&IDLength, 1, 1, out);
    fwrite(&cMapType, 1, 1, out);
    fwrite(&imageType, 1, 1, out);

    fwrite(&cMapIndex, 2, 1, out);
    fwrite(&cMapLength, 2, 1, out);
    fwrite(&cMapS, 1, 1, out);

    fwrite(&xOrigin, 2, 1, out);
    fwrite(&yOrigin, 2, 1, out);
    fwrite(&width, 2, 1, out);
    fwrite(&height, 2, 1, out);
    fwrite(&depth, 1, 1, out);
    fwrite(&descriptor, 1, 1, out);



    // Store the image run-length encoded

    int y = BN;         // our current position
    int x = BW + 1;

    int l = 1;          // the length of the current run
    int v = P[y*S + 1]; // the value of the current run

    while (y <= BS && x <= BE) {
        int i = y*S + x;

        if (P[i] != v || l == 127) {
            writeRun(out, v, l);
            
            l = 1;
            v = P[i];
        }
        else {
            l++;
        }

        if (++x > BE) {
            x = BW;
            y++;
        }
    }
    writeRun(out, v, l);

    fclose(out);

    free(P);
}