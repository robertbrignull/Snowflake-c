#include <math.h>

#include "algo.h"

int max(int x, int y) {
    return (x > y) ? x : y;
}

int min(int x, int y) {
    return (x < y) ? x : y;
}

double dist(double x, double y, double u, double v) {
   return sqrt((x-u)*(x-u) + (y-v)*(y-v));
}

double dist_origin(double x, double y) {
    return sqrt(x*x + y*y);
}
