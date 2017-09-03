#pragma once

#include <stdio.h>
#include <math.h>

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif

#define CHECK_MEM(m) if (m == 0) { printf("Memory allocation failure.\n"); exit(1); }

inline int max(int x, int y) {
    return (x > y) ? x : y;
}

inline int min(int x, int y) {
    return (x < y) ? x : y;
}

inline double dist(double x, double y, double u, double v) {
    return sqrt((x-u)*(x-u) + (y-v)*(y-v));
}

inline double dist_origin(double x, double y) {
    return sqrt(x*x + y*y);
}