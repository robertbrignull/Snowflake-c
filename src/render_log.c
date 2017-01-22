#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "algo.h"
#include "write_png.h"

#include "render_log.h"
#include "flake_log.h"

typedef struct image_def {
    unsigned char *P;      // the pixels array
    size_t P_size;   // the length of the pixels array
    int width;    // the width of the image
    int height;   // the height of the image
    int left;     // the left offset of the image in the coordinate space
    int top;      // the top offset of the image in the coordinate space
} image_def;

image_def create_image_array(double *points, int num_particles, int colorize) {
    int BN = 0;
    int BS = 0;
    int BW = 0;
    int BE = 0;

    for (int i = 0; i < num_particles; i++) {
        int x = (int) floor(points[2*i]);
        int y = (int) floor(points[2*i+1]);

        BN = min(BN, y);
        BS = max(BS, y);
        BW = min(BW, x);
        BE = max(BE, x);
    }

    image_def image;
    image.width = BE - BW + 1;
    image.height = BS - BN + 1;
    image.left = BW;
    image.top = BN;

    // Add a border for prettiness
    image.width += 20;
    image.height += 20;
    image.left -= 10;
    image.top -= 10;

    // make the width and height a multiple of 2 to play nicely with ffmpeg
    if (image.width % 2 == 1) image.width += 1;
    if (image.height % 2 == 1) image.height += 1;

    image.P_size = (size_t) image.width * image.height * bytes_per_pixel(colorize);
    image.P = (unsigned char*) malloc(image.P_size);
    CHECK_MEM(image.P);

    return image;
}

void hsv2rgb(double hue, unsigned char *r, unsigned char *g, unsigned char *b) {
    double hue_range = fmod(hue / 60.0, 6);
    unsigned char x = (unsigned char) (255 * (1.0 - fabs(fmod(hue_range, 2.0) - 1.0)));

    if (hue_range < 1) {
        *r = (unsigned char) 255; *g = x; *b = 0;
    }
    else if (hue_range < 2) {
        *r = x; *g = (unsigned char) 255; *b = 0;
    }
    else if (hue_range < 3) {
        *r = 0; *g = (unsigned char) 255; *b = x;
    }
    else if (hue_range < 4) {
        *r = 0; *g = x; *b = (unsigned char) 255;
    }
    else if (hue_range < 5) {
        *r = x; *g = 0; *b = (unsigned char) 255;
    }
    else {
        *r = (unsigned char) 255; *g = 0; *b = x;
    }
}

void generate_image(double *points, image_def image, int colorize, int limit) {
    memset(image.P, 0, image.P_size);

    for (int i = 0; i < limit; i++) {
        int x = ((int) floor(points[2*i])) - image.left;
        int y = ((int) floor(points[2*i+1])) - image.top;

        if (x >= 0 && x < image.width && y >= 0 && y < image.height) {
            int index = pix(x, y, image.width, image.height, colorize);
            if (colorize) {
                double age = ((double) i * 360) / limit;
                hsv2rgb(age, &image.P[index+0], &image.P[index+1], &image.P[index+2]);
            }
            else {
                image.P[index] = (unsigned char) 255;
            }
        }
    }
}

void render_log(FILE *log, char *filename, int colorize, int movie, int num_frames) {
    int num_particles;
    double *points = read_flake_log_as_array(log, &num_particles);
    if (points == NULL) {
        fprintf(stderr, "Unable to render log");
        return;
    }

    image_def image = create_image_array(points, num_particles, colorize);

    if (movie) {
        char *frame_filename = (char*) malloc(strlen(filename) + 100);
        for (int frame = 0; frame < num_frames; frame++) {
            int limit = (int) (1.0 * num_particles * frame / num_frames);
            sprintf(frame_filename, filename, limit);
            generate_image(points, image, colorize, limit);
            write_png(frame_filename, image.P, image.width, image.height, colorize);
        }
    }
    else {
        generate_image(points, image, colorize, num_particles);
        write_png(filename, image.P, image.width, image.height, colorize);
    }
   
    free(points);
    free(image.P);
}
