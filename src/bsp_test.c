#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>

#include "bsp.h"

#include "bsp_test.h"

void test_basic_distances() {
    printf("Starting test_basic_distances...");

    bsp_t *b = bsp_new(100.0);

    assert(bsp_find_nearest(b, 0.0, 0.0).d == -1.0);

    bsp_add_point(b, 1.0, 2.0);
    bsp_result d = bsp_find_nearest(b, 0.0, 0.0);
    assert(d.d > 2.236 && d.d < 2.237);

    bsp_add_point(b, -1.0, 0.0);
    d = bsp_find_nearest(b, 0.0, 0.0);
    assert(d.d > 0.999 && d.d < 1.001);

    bsp_destroy(b);

    printf(" Done\n");
}

void test_crossing_boundaries() {
    printf("Starting test_crossing_boundaries...");

    bsp_t *b = bsp_new(100.0);

    assert(bsp_find_nearest(b, 50.0, 50.0).d == -1.0);

    bsp_add_point(b, 45.0, 26.0);
    bsp_add_point(b, 49.0, 24.0);

    bsp_result d = bsp_find_nearest(b, 45.0, 24.0);
    assert(d.d > 1.999 && d.d < 2.001);

    bsp_destroy(b);

    printf(" Done\n");
}

void test_point_added_twice() {
    printf("Starting test_point_added_twice...");

    bsp_t *b = bsp_new(100.0);

    assert(bsp_find_nearest(b, 50.0, 50.0).d == -1.0);

    bsp_add_point(b, 40.0, 40.0);
    bsp_add_point(b, 40.0, 40.0);

    bsp_result d = bsp_find_nearest(b, 38.0, 40.0);
    assert(d.d > 1.999 && d.d < 2.001);

    bsp_destroy(b);

    printf(" Done\n");
}

void test_empty_region() {
    printf("Starting test_empty_region...");

    bsp_t *b = bsp_new(100.0);

    assert(bsp_find_nearest(b, 50.0, 50.0).d == -1.0);

    bsp_add_point(b, 25.0, 25.0);
    bsp_add_point(b, 25.0, 60.0);

    bsp_result d = bsp_find_nearest(b, 75.0, 25.0);
    assert(d.d > 49.999 && d.d < 50.001);

    d = bsp_find_nearest(b, 55.0, 60.0);
    assert(d.d > 29.999 && d.d < 30.001);

    d = bsp_find_nearest(b, 55.0, 45.0);
    assert(d.d > 33.541 && d.d < 33.542);

    bsp_destroy(b);

    printf(" Done\n");
}

void test_performance() {
    printf("Starting test_performance... ");

    int N = 1000;
    int x, y;
    struct timeval s, e;
    __time_t t;
    bsp_result d;

    printf("N = %d, ", N);

    bsp_t *b = bsp_new(N);

    gettimeofday(&s, 0);
    for (x = 0; x < N; x++) {
        for (y = 0; y < N; y++) {
            bsp_add_point(b, x, y);
        }
    }
    gettimeofday(&e, 0);
    t = (e.tv_sec * 1000000 + e.tv_usec) - (s.tv_sec * 1000000 + s.tv_usec);
    printf("%fus per addition, ", 1.0 * t / (N*N));

    gettimeofday(&s, 0);
    for (x = 0; x < N; x++) {
        for (y = 0; y < N; y++) {
            d = bsp_find_nearest(b, x+0.5, y+0.5);
            assert(d.d > 0.707 && d.d < 0.708);
        }
    }
    gettimeofday(&e, 0);
    t = (e.tv_sec * 1000000 + e.tv_usec) - (s.tv_sec * 1000000 + s.tv_usec);
    printf("%fus per find ", 1.0 * t / (N*N));

    bsp_destroy(b);

    printf("... Done\n");
}

void test_random() {
    printf("Starting test_random... ");

    int N = 1000000;
    double w = 1000.0;

    int n;
    double x, y;

    struct timeval s, e;
    __time_t t;

    srand(42);

    printf("N = %d, ", N);

    bsp_t *b = bsp_new(w);

    gettimeofday(&s, 0);
    for (n = 0; n < N; n++) {
        x = w * rand() / RAND_MAX;
        y = w * rand() / RAND_MAX;
        bsp_add_point(b, x, y);
    }
    gettimeofday(&e, 0);
    t = (e.tv_sec * 1000000 + e.tv_usec) - (s.tv_sec * 1000000 + s.tv_usec);
    printf("%fus per addition, ", 1.0 * t / N);

    gettimeofday(&s, 0);
    for (n = 0; n < N; n++) {
        x = w * rand() / RAND_MAX;
        y = w * rand() / RAND_MAX;
        bsp_find_nearest(b, x, y);
    }
    gettimeofday(&e, 0);
    t = (e.tv_sec * 1000000 + e.tv_usec) - (s.tv_sec * 1000000 + s.tv_usec);
    printf("%fus per find ", 1.0 * t / N);

    bsp_destroy(b);

    printf("... Done\n");
}

void run_bsp_tests() {
    test_basic_distances();
    test_crossing_boundaries();
    test_point_added_twice();
    test_empty_region();

    test_performance();
    test_random();
}
