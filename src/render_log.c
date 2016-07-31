#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "algo.h"
#include "tga.h"

#include "render_log.h"

int find_number_of_particles(FILE *log) {
    fseek(log, 0, SEEK_SET);

    int lines = 1;
    int ch = 0;
    while ((ch = fgetc(log)) != EOF)
    {
        if (ch == '\n') lines++;
    }
    return lines;
}

void find_bounds(FILE *log, int *BN, int *BS, int *BW, int *BE) {
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

void hsv2rgb(double hue, char *r, char *g, char *b) {
    char x = 255 * (((int) floor(hue / 60)) % 2);
    if (hue < 60) {
        *r = 255; *g = x; *b = 0;
    }
    else if (hue < 120) {
        *r = x; *g = 255; *b = 0;
    }
    else if (hue < 180) {
        *r = 0; *g = 255; *b = x;
    }
    else if (hue < 240) {
        *r = 0; *g = x; *b = 255;
    }
    else if (hue < 300) {
        *r = x; *g = 0; *b = 255;
    }
    else {
        *r = 255; *g = 0; *b = x;
    }
}

char *generate_image(FILE *log, int *w, int *h, int colorize) {
    int BN, BS, BW, BE;
    find_bounds(log, &BN, &BS, &BW, &BE);

    int width = BE - BW + 1;
    int height = BS - BN + 1;
    int left = BW;
    int top = BN;

    int P_size = width * height * bytes_per_pixel(colorize);
    char *P = (char*) malloc(P_size);
    CHECK_MEM(P);
    memset(P, 0, P_size);

    int num_particles = 0;
    if (colorize) {
        num_particles = find_number_of_particles(log);
    }

    fseek(log, 0, SEEK_SET);

    int n = 0;
    double d_x, d_y;
    while (fscanf(log, "%*d %lf %lf %*d", &d_x, &d_y) == 2) {
        n += 1;

        int x = ((int) floor(d_x)) - left;
        int y = ((int) floor(d_y)) - top;

        if (x >= 0 && x < width && y >= 0 && y < height) {
            int index = pix(x, y, width, height, colorize);
            if (colorize) {
                double age = ((double) n * 360) / num_particles;
                hsv2rgb(age, &P[index+0], &P[index+1], &P[index+2]);
            }
            else {
                P[index] = 255;
            }
        }
    }

    *w = width;
    *h = height;

    return P;
}

void render_log(FILE *log, char *filename, int colorize) {
    // Write the flake to a 2D array of pixels
    int width, height;
    char *P = generate_image(log, &width, &height, colorize);

    write_tga(filename, P, width, height, colorize);

    free(P);
}
