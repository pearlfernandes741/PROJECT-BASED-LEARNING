
#include <stdio.h>
#include <time.h>

#define OPERATIONS 1000

int main(void)
{
    struct timespec start, end;
    volatile double result = 0.0;
    const double a = 3.0, b = 5.0;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < OPERATIONS; i++) {
        result = a + b;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed =
        (end.tv_sec - start.tv_sec) +
        (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("========================================\n");
    printf(" SEQUENTIAL BASELINE BENCHMARK\n");
    printf("========================================\n");
    printf("Operations: %d additions\n", OPERATIONS);
    printf("Final result: %.0f\n", result);
    printf("Execution time: %.9f seconds\n", elapsed);
    printf("Execution time: %.3f milliseconds\n", elapsed * 1000.0);
    printf("========================================\n");

    return 0;
}
