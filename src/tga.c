#include <stdio.h>
#include <stdlib.h>

#include "tga.h"

int pix(int x, int y, int width, int height) {
    return x * height + y;
}

void write_run(FILE *out, int v, int l) {
    unsigned char header = 127 + l;
    unsigned char data = v;

    fwrite(&header, 1, 1, out);
    fwrite(&data, 1, 1, out);
}

void write_tga(char *filename, char *pixels,
               unsigned short width, unsigned short height) {
    FILE *out = fopen(filename, "w");

    unsigned char id_length = 0;
    unsigned char color_map_type = 0;
    unsigned char image_type = 11;

    unsigned short color_map_index = 0;
    unsigned short color_map_length = 0;
    unsigned char color_map_entry_size = 0;

    unsigned short x_origin = 0;
    unsigned short y_origin = 0;
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

    // the length and value of the current run
    int l = 0;
    int v = pixels[pix(0, 0, width, height)];

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int i = pix(x, y, width, height);

            if (pixels[i] != v || l == 127) {
                write_run(out, v, l);
                
                l = 1;
                v = pixels[i];
            }
            else {
                l++;
            }
        }
    }
    write_run(out, v, l);

    fclose(out);
}
