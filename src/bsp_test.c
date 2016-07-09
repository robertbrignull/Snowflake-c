#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <assert.h>

#include "bsp.h"

void test_basicDistances() {
    printf("Starting test_basicDistances...");

    BSP_t *b = BSP_new(100.0);

    assert(BSP_findNearest(b, 0.0, 0.0).d == 0.0);

    BSP_addPoint(b, 51.0, 52.0);

    BSP_result d = BSP_findNearest(b, 50.0, 50.0);
    assert(d.d > 2.236 && d.d < 2.237);

    BSP_addPoint(b, 49.0, 50.0);
    d = BSP_findNearest(b, 50.0, 50.0);
    assert(d.d > 0.999 && d.d < 1.001);

    BSP_destroy(b);

    printf(" Done\n");
}

void test_crossingBoundaries() {
    printf("Starting test_crossingBoundaries...");

    BSP_t *b = BSP_new(100.0);

    assert(BSP_findNearest(b, 50.0, 50.0).d == 0.0);

    BSP_addPoint(b, 45.0, 26.0);
    BSP_addPoint(b, 49.0, 24.0);

    BSP_result d = BSP_findNearest(b, 45.0, 24.0);
    assert(d.d > 1.999 && d.d < 2.001);

    BSP_destroy(b);

    printf(" Done\n");
}

void test_pointAddedTwice() {
    printf("Starting test_pointAddedTwice...");

    BSP_t *b = BSP_new(100.0);

    assert(BSP_findNearest(b, 50.0, 50.0).d == 0.0);

    BSP_addPoint(b, 40.0, 40.0);
    BSP_addPoint(b, 40.0, 40.0);

    BSP_result d = BSP_findNearest(b, 38.0, 40.0);
    assert(d.d > 1.999 && d.d < 2.001);

    BSP_destroy(b);

    printf(" Done\n");
}

void test_emptyRegion() {
    printf("Starting test_emptyRegion...");

    BSP_t *b = BSP_new(100.0);

    assert(BSP_findNearest(b, 50.0, 50.0).d == 0.0);

    BSP_addPoint(b, 25.0, 25.0);
    BSP_addPoint(b, 25.0, 60.0);

    BSP_result d = BSP_findNearest(b, 75.0, 25.0);
    assert(d.d > 49.999 && d.d < 50.001);

    d = BSP_findNearest(b, 55.0, 60.0);
    assert(d.d > 29.999 && d.d < 30.001);

    d = BSP_findNearest(b, 55.0, 45.0);
    assert(d.d > 33.541 && d.d < 33.542);

    BSP_destroy(b);

    printf(" Done\n");
}

void test_performance() {
    printf("Starting test_performance... ");

    int N = 1000;
    int x, y;
    struct timeval s, e;
    int t;
    BSP_result d;

    printf("N = %d, ", N);

    BSP_t *b = BSP_new(N);

    gettimeofday(&s, 0);
    for (x = 0; x < N; x++) {
        for (y = 0; y < N; y++) {
            BSP_addPoint(b, x, y);
        }
    }
    gettimeofday(&e, 0);
    t = (e.tv_sec * 1000000 + e.tv_usec) - (s.tv_sec * 1000000 + s.tv_usec);
    printf("%fus per addition, ", 1.0 * t / (N*N));

    gettimeofday(&s, 0);
    for (x = 0; x < N; x++) {
        for (y = 0; y < N; y++) {
            d = BSP_findNearest(b, x+0.5, y+0.5);
            assert(d.d > 0.707 && 0.708);
        }
    }
    gettimeofday(&e, 0);
    t = (e.tv_sec * 1000000 + e.tv_usec) - (s.tv_sec * 1000000 + s.tv_usec);
    printf("%fus per find ", 1.0 * t / (N*N));

    BSP_destroy(b);

    printf("... Done\n");
}

void test_random() {
    printf("Starting test_random... ");

    int N = 1000000;
    double w = 1000.0;

    int n;
    double x, y;

    struct timeval s, e;
    int t;

    srand(42);

    printf("N = %d, ", N);

    BSP_t *b = BSP_new(w);

    gettimeofday(&s, 0);
    for (n = 0; n < N; n++) {
        x = w * rand() / RAND_MAX;
        y = w * rand() / RAND_MAX;
        BSP_addPoint(b, x, y);
    }
    gettimeofday(&e, 0);
    t = (e.tv_sec * 1000000 + e.tv_usec) - (s.tv_sec * 1000000 + s.tv_usec);
    printf("%fus per addition, ", 1.0 * t / N);

    gettimeofday(&s, 0);
    for (n = 0; n < N; n++) {
        x = w * rand() / RAND_MAX;
        y = w * rand() / RAND_MAX;
        BSP_findNearest(b, x, y);
    }
    gettimeofday(&e, 0);
    t = (e.tv_sec * 1000000 + e.tv_usec) - (s.tv_sec * 1000000 + s.tv_usec);
    printf("%fus per find ", 1.0 * t / N);

    BSP_destroy(b);

    printf("... Done\n");
}

void test_iterator() {
    printf("Starting test_iterator");

    BSP_t *b = BSP_new(40.0);

    double points[3] = { 10.0, 20.0, 30.0 };
    int found[3] = { 0, 0, 0 };

    for (int i = 0; i < 3; i++) {
        BSP_addPoint(b, points[i], points[i]);
    }

    BSP_iterator it = BSP_iterator_new(b);

    for (int i = 0; i < 3; i++) {
        assert (BSP_iterator_hasNext(it) != 0);
        BSP_result r = BSP_iterator_next(&it);

        int f = 0;
        for (int j = 0; j < 3; j++) {
            if (r.x == points[j]) {
                assert (found[j] == 0);
                assert (f == 0);
                found[j] = 1;
                f = 1;
            }
        }
        assert (f == 1);
    }

    assert (BSP_iterator_hasNext(it) == 0);

    for (int i = 0; i < 3; i++) {
        assert (found[i] == 1);
    }

    BSP_destroy(b);

    printf("... Done\n");
}

void test_iteratorLength() {
    printf("Starting test_iteratorLength");

    BSP_t *b = BSP_new(40.0);

    srand(42);

    int n = 10000;

    for (int i = 0; i < n; i++) {
        BSP_addPoint(b, 1.0 * rand() / RAND_MAX, 1.0 * rand() / RAND_MAX);
    }

    BSP_iterator it = BSP_iterator_new(b);

    int k = 0;
    while (BSP_iterator_hasNext(it) != 0) {
        BSP_iterator_next(&it);
        k++;
    }

    assert (k == n);

    BSP_destroy(b);

    printf("... Done\n");
}

int main(int argc, char **argv) {
    // Limit memory usage to around 4GB
    struct rlimit lim;
    lim.rlim_cur = lim.rlim_max = 4000000000;
    setrlimit(RLIMIT_AS, &lim);

    test_basicDistances();
    test_crossingBoundaries();
    test_pointAddedTwice();
    test_emptyRegion();

    test_performance();
    test_random();

    test_iterator();
    test_iteratorLength();

    return 0;
}
