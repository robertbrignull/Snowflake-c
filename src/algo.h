#pragma once

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif

#define CHECK_MEM(m) if (m == 0) { printf("Memory allocation failure.\n"); exit(1); }

int min_i(int x, int y);

int max_i(int x, int y);

double max_d(double x, double y);

double min_d(double x, double y);

double abs_d(double x);

double dist_d(double x, double y, double u, double v);

double dist_origin_d(double x, double y);
