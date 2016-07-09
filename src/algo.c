#include <math.h>

#include "algo.h"

double max_d(double x, double y) {
    return (x > y) ? x : y;
}

double min_d(double x, double y) {
    return (x < y) ? x : y;
}

double abs_d(double x) {
    return (x >= 0) ? x : -x;
}

double dist_d(double x, double y, double u, double v) {
   return sqrt((x-u)*(x-u) + (y-v)*(y-v));
}