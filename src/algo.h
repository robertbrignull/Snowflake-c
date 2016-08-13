#pragma once

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif

#define CHECK_MEM(m) if (m == 0) { printf("Memory allocation failure.\n"); exit(1); }

int min(int x, int y);

int max(int x, int y);

double dist(double x, double y, double u, double v);

double dist_origin(double x, double y);
