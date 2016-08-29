#include <stdio.h>
#include <stdlib.h>
#include <png.h>

#include "write_png.h"

int bytes_per_pixel(int color) {
    return color ? 3 : 1;
}

int pix(int x, int y, int width, int height, int color) {
    return (x * height + y) * bytes_per_pixel(color);
}

// Expects pixels to be of length width * height if greyscale or
// or length width * height * 3 if color.
void write_png(char *filename, unsigned char *pixels, int width, int height, int color) {
    int code = 0;
    FILE *fp = 0;
    png_structp png_ptr = 0;
    png_infop info_ptr = 0;
    png_bytep row = 0;

    // Open output file
    fp = fopen(filename, "wb");
    if (fp == 0) {
        fprintf(stderr, "Could not open \"%s\" for writing\n", filename);
        code = 1;
        goto finalise;
    }

    // Initialise the write structure
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (png_ptr == 0) {
        fprintf(stderr, "Could not allocate png write struct\n");
        code = 1;
        goto finalise;
    }

    // Initialise the info structure
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == 0) {
        fprintf(stderr, "Could not allocate info struct\n");
        code = 1;
        goto finalise;
    }

    // Setup Exception handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Error during png creation\n");
        code = 1;
        goto finalise;
    }

    png_init_io(png_ptr, fp);

    int color_type = (color) ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_GRAY;

    // Write header
    png_set_IHDR(png_ptr, info_ptr, (png_uint_32) width, (png_uint_32) height,
                 8, color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    // Allocate memory for one row - 3 bytes per pixel
    row = (png_bytep) malloc(bytes_per_pixel(color) * width * sizeof(png_byte));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int row_index = x * bytes_per_pixel(color);
            int pixels_index = pix(x, y, width, height, color);

            row[row_index] = pixels[pixels_index];
            row[row_index + 1] = pixels[pixels_index + 1];
            row[row_index + 2] = pixels[pixels_index + 2];
        }
        png_write_row(png_ptr, row);
    }

    // End writing
    png_write_end(png_ptr, 0);

    finalise:
    if (fp != NULL) fclose(fp);
    if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
    if (row != NULL) free(row);

    if (code != 0) exit(code);
}
