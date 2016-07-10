#pragma once

/**
 * Maps a pixel's coordinates from x,y to what its
 * position should be in a 1D array.
 */
int pix(int x, int y, int width, int height);

/**
 * Creata .tga image file from the given pixels where
 * the index of a pixel within the 1D char array is
 * given by the function pix above.
 */
void write_tga(char *filename, char *pixels,
               unsigned short width, unsigned short height);
