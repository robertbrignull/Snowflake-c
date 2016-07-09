#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "bsp.h"
#include "algo.h"

char *generate_image(bsp_t *b, int S) {
    char *P = (char*) malloc(S*S);
    CHECK_MEM(P);

    memset(P, 0, S*S);

    bsp_iterator it = bsp_iterator_new(b);

    while (bsp_iterator_has_next(it)) {
        bsp_result r = bsp_iterator_next(&it);

        int x = (int) floor(r.x);
        int y = (int) floor(r.y);

        P[y*S + x] = 255;
    }

    return P;
}

void write_run(FILE *out, int v, int l) {
    unsigned char header = 127 + l;
    unsigned char data = v;

    fwrite(&header, 1, 1, out);
    fwrite(&data, 1, 1, out);
}

void write_image(bsp_t *b, char *filename) {
    int S = (int) ceil(b->S);

    // Write the flake to a 2D array of pixels
    char *P = generate_image(b, S);

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

    unsigned char id_length = 0;
    unsigned char color_map_type = 0;
    unsigned char image_type = 11;

    unsigned short color_map_index = 0;
    unsigned short color_map_length = 0;
    unsigned char color_map_entry_size = 0;

    unsigned short x_origin = 0;
    unsigned short y_origin = 0;
    unsigned short width = BE - BW + 1;
    unsigned short height = BS - BN + 1;
    unsigned char depth = 8;
    unsigned char descriptor = 0;



    fwrite(&id_length, 1, 1, out);
    fwrite(&color_map_type, 1, 1, out);
    fwrite(&image_type, 1, 1, out);

    fwrite(&color_map_index, 2, 1, out);
    fwrite(&color_map_length, 2, 1, out);
    fwrite(&color_map_entry_size, 1, 1, out);

    fwrite(&x_origin, 2, 1, out);
    fwrite(&y_origin, 2, 1, out);
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
            write_run(out, v, l);
            
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
    write_run(out, v, l);

    fclose(out);

    free(P);
}
