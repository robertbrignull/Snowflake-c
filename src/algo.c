#include <math.h>

#include "algo.h"

int max_i(int x, int y) {
    return (x > y) ? x : y;
}

int min_i(int x, int y) {
    return (x < y) ? x : y;
}

double dist_d(double x, double y, double u, double v) {
   return sqrt((x-u)*(x-u) + (y-v)*(y-v));
}

double dist_origin_d(double x, double y) {
    return sqrt(x*x + y*y);
}
