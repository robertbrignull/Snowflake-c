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

typedef struct image_def {
    char *P;      // the pixels array
    int P_size;   // the length of the pixels array
    int width;    // the width of the image
    int height;   // the height of the image
    int left;     // the left offset of the image in the coordinate space
    int top;      // the top offset of the image in the coordinate space
} image_def;

image_def create_array(FILE *log, int colorize) {
    int BN = 0;
    int BS = 0;
    int BW = 0;
    int BE = 0;

    fseek(log, 0, SEEK_SET);

    double d_x, d_y;
    while (fscanf(log, "%*d %lf %lf %*d", &d_x, &d_y) == 2) {
        int x = (int) floor(d_x);
        int y = (int) floor(d_y);

        BN = min_i(BN, y);
        BS = max_i(BS, y);
        BW = min_i(BW, x);
        BE = max_i(BE, x);
    }

    image_def image;
    image.width = BE - BW + 1;
    image.height = BS - BN + 1;
    image.left = BW;
    image.top = BN;

    // make the width and height a multiple of 2 to play nicely with ffmpeg
    if (image.width % 2 == 1) image.width += 1;
    if (image.height % 2 == 1) image.height += 1;

    image.P_size = image.width * image.height * bytes_per_pixel(colorize);
    image.P = (char*) malloc(image.P_size);
    CHECK_MEM(image.P);

    return image;
}

void hsv2rgb(double hue, char *r, char *g, char *b) {
    char x = 255 * fmod(hue, 60.0) / 60;
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

void generate_image(FILE *log, image_def image, int colorize, int limit) {
    memset(image.P, 0, image.P_size);

    fseek(log, 0, SEEK_SET);

    int n = 1;
    double d_x, d_y;
    while (fscanf(log, "%*d %lf %lf %*d", &d_x, &d_y) == 2 && n < limit) {
        n += 1;

        int x = ((int) floor(d_x)) - image.left;
        int y = ((int) floor(d_y)) - image.top;

        if (x >= 0 && x < image.width && y >= 0 && y < image.height) {
            int index = pix(x, y, image.width, image.height, colorize);
            if (colorize) {
                double age = ((double) n * 360) / limit;
                hsv2rgb(age, &image.P[index+0], &image.P[index+1], &image.P[index+2]);
            }
            else {
                image.P[index] = 255;
            }
        }
    }
}

void render_up_to_limit(FILE *log, char *filename, image_def image, int colorize, int limit) {
    generate_image(log, image, colorize, limit);
    write_tga(filename, image.P, image.width, image.height, colorize);
}

void render_log(FILE *log, char *filename, int colorize, int movie, int num_frames) {
    int num_particles = find_number_of_particles(log);

    image_def image = create_array(log, colorize);

    if (movie) {
        char *frame_filename = (char*) malloc(strlen(filename) + 100);
        for (int frame = 0; frame < num_frames; frame++) {
            int limit = 1.0 * num_particles * frame / num_frames;
            sprintf(frame_filename, filename, limit);
            render_up_to_limit(log, frame_filename, image, colorize, limit);
        }
    }
    else {
        render_up_to_limit(log, filename, image, colorize, num_particles);
    }
   
    free(image.P);
}
