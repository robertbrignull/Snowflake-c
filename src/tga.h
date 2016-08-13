#pragma once

/**
 * Returns the number of bytes that should be used
 * to encode each pixel when color or greyscale.
 */
int bytes_per_pixel(int color);

/**
 * Maps a pixel's coordinates from x,y to what its
 * position should be in a 1D array.
 */
int pix(int x, int y, int width, int height, int color);

/**
 * Creata .tga image file from the given pixels where
 * the index of a pixel within the 1D char array is
 * given by the function pix above.
 */
void write_tga(char *filename, char *pixels, int width, int height, int color);
