#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tga.h"

int bytes_per_pixel(int color) {
    return color ? 3 : 1;
}

int pix(int x, int y, int width, int height, int color) {
    return (x * height + y) * bytes_per_pixel(color);
}

int pixels_match(char *pixels, int pix1, int pix2, int color) {
    return memcmp(&pixels[pix1], &pixels[pix2], (size_t) bytes_per_pixel(color)) == 0;
}

void write_run(FILE *out, char *pixels, int index, int l, int color) {
    unsigned char header = (unsigned char) (127 + l);
    fwrite(&header, 1, 1, out);

    for (int i = 0; i < bytes_per_pixel(color); i++) {
        unsigned char data = pixels[index + i];
        fwrite(&data, 1, 1, out);
    }
}

// Expects pixels to be of length width * height if greyscale or
// or length width * height * 3 if color.
void write_tga(char *filename, char *pixels, int width, int height, int color) {
    FILE *out = fopen(filename, "w");

    if (out == 0) {
        fprintf(stderr, "Could not open \"%s\" for writing\n", filename);
        exit(1);
    }

    unsigned char id_length = 0;
    unsigned char color_map_type = 0;
    unsigned char image_type = (unsigned char) (color ? 10 : 11);

    unsigned short color_map_index = 0;
    unsigned short color_map_length = 0;
    unsigned char color_map_entry_size = 0;

    unsigned short x_origin = 0;
    unsigned short y_origin = 0;
    unsigned short short_width = (unsigned short) width;
    unsigned short short_height = (unsigned short) height;
    unsigned char depth = (unsigned char) (bytes_per_pixel(color) * 8);
    unsigned char descriptor = 0;



    fwrite(&id_length, 1, 1, out);
    fwrite(&color_map_type, 1, 1, out);
    fwrite(&image_type, 1, 1, out);

    fwrite(&color_map_index, 2, 1, out);
    fwrite(&color_map_length, 2, 1, out);
    fwrite(&color_map_entry_size, 1, 1, out);

    fwrite(&x_origin, 2, 1, out);
    fwrite(&y_origin, 2, 1, out);
    fwrite(&short_width, 2, 1, out);
    fwrite(&short_height, 2, 1, out);
    fwrite(&depth, 1, 1, out);
    fwrite(&descriptor, 1, 1, out);



    // Store the image run-length encoded

    // the length and index of the current run
    int l = 0;
    int c = pix(0, 0, width, height, color);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int i = pix(x, y, width, height, color);

            if (!pixels_match(pixels, c, i, color) || l == 127) {
                write_run(out, pixels, c, l, color);
                
                l = 1;
                c = i;
            }
            else {
                l++;
            }
        }
    }
    write_run(out, pixels, c, l, color);

    fclose(out);
}
