#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>

#include "bsp.h"
#include "flake.h"

#include "bsp_test.h"

void test_create_bsp() {
    printf("Starting test_create_bsp...");

    bsp_t *b = new_flake(100.0);

    destroy_flake(b);

    printf(" Done\n");
}

void test_add_single_point() {
    printf("Starting test_create_bsp...");

    bsp_t *b = new_flake(100.0);

    add_point_to_flake(b, 1.0, 2.0);

    destroy_flake(b);

    printf(" Done\n");
}

void test_add_multiple_point() {
    printf("Starting test_create_bsp...");

    bsp_t *b = new_flake(100.0);

    add_point_to_flake(b, 1.0, 2.0);
    add_point_to_flake(b, -1.0, 0.0);

    destroy_flake(b);

    printf(" Done\n");
}

void test_basic_distances() {
    printf("Starting test_basic_distances...");

    bsp_t *b = new_flake(100.0);

    assert(find_nearest_in_flake(b, 0.0, 0.0).d == -1.0);

    add_point_to_flake(b, 1.0, 2.0);
    bsp_result d = find_nearest_in_flake(b, 0.0, 0.0);
    assert(d.d > 2.236 && d.d < 2.237);

    add_point_to_flake(b, -1.0, 0.0);
    d = find_nearest_in_flake(b, 0.0, 0.0);
    assert(d.d > 0.999 && d.d < 1.001);

    destroy_flake(b);

    printf(" Done\n");
}

void test_crossing_boundaries() {
    printf("Starting test_crossing_boundaries...");

    bsp_t *b = new_flake(100.0);

    assert(find_nearest_in_flake(b, 50.0, 50.0).d == -1.0);

    add_point_to_flake(b, 45.0, 26.0);
    add_point_to_flake(b, 49.0, 24.0);

    bsp_result d = find_nearest_in_flake(b, 45.0, 24.0);
    assert(d.d > 1.999 && d.d < 2.001);

    destroy_flake(b);

    printf(" Done\n");
}

void test_point_added_twice() {
    printf("Starting test_point_added_twice...");

    bsp_t *b = new_flake(100.0);

    assert(find_nearest_in_flake(b, 50.0, 50.0).d == -1.0);

    add_point_to_flake(b, 40.0, 40.0);
    add_point_to_flake(b, 40.0, 40.0);

    bsp_result d = find_nearest_in_flake(b, 38.0, 40.0);
    assert(d.d > 1.999 && d.d < 2.001);

    destroy_flake(b);

    printf(" Done\n");
}

void test_empty_region() {
    printf("Starting test_empty_region...");

    bsp_t *b = new_flake(100.0);

    assert(find_nearest_in_flake(b, 50.0, 50.0).d == -1.0);

    add_point_to_flake(b, 25.0, 25.0);
    add_point_to_flake(b, 25.0, 60.0);

    bsp_result d = find_nearest_in_flake(b, 75.0, 25.0);
    assert(d.d > 49.999 && d.d < 50.001);

    d = find_nearest_in_flake(b, 55.0, 60.0);
    assert(d.d > 29.999 && d.d < 30.001);

    d = find_nearest_in_flake(b, 55.0, 45.0);
    assert(d.d > 33.541 && d.d < 33.542);

    destroy_flake(b);

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

    bsp_t *b = new_flake(N);

    gettimeofday(&s, 0);
    for (x = 0; x < N; x++) {
        for (y = 0; y < N; y++) {
            add_point_to_flake(b, x, y);
        }
    }
    gettimeofday(&e, 0);
    t = (e.tv_sec * 1000000 + e.tv_usec) - (s.tv_sec * 1000000 + s.tv_usec);
    printf("%fus per addition, ", 1.0 * t / (N*N));

    gettimeofday(&s, 0);
    for (x = 0; x < N; x++) {
        for (y = 0; y < N; y++) {
            d = find_nearest_in_flake(b, x + 0.5, y + 0.5);
            assert(d.d > 0.707 && d.d < 0.708);
        }
    }
    gettimeofday(&e, 0);
    t = (e.tv_sec * 1000000 + e.tv_usec) - (s.tv_sec * 1000000 + s.tv_usec);
    printf("%fus per find ", 1.0 * t / (N*N));

    destroy_flake(b);

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

    bsp_t *b = new_flake(w);

    gettimeofday(&s, 0);
    for (n = 0; n < N; n++) {
        x = w * rand() / RAND_MAX;
        y = w * rand() / RAND_MAX;
        add_point_to_flake(b, x, y);
    }
    gettimeofday(&e, 0);
    t = (e.tv_sec * 1000000 + e.tv_usec) - (s.tv_sec * 1000000 + s.tv_usec);
    printf("%fus per addition, ", 1.0 * t / N);

    gettimeofday(&s, 0);
    for (n = 0; n < N; n++) {
        x = w * rand() / RAND_MAX;
        y = w * rand() / RAND_MAX;
        find_nearest_in_flake(b, x, y);
    }
    gettimeofday(&e, 0);
    t = (e.tv_sec * 1000000 + e.tv_usec) - (s.tv_sec * 1000000 + s.tv_usec);
    printf("%fus per find ", 1.0 * t / N);

    destroy_flake(b);

    printf("... Done\n");
}

void run_bsp_tests() {
    test_create_bsp();
    test_add_single_point();
    test_add_multiple_point();
    test_basic_distances();
    test_crossing_boundaries();
    test_point_added_twice();
    test_empty_region();

    test_performance();
    test_random();
}
