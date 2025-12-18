#include <omp.h>
#include <stdio.h>
#include "../utils/utils.h"

double fn(int i) {
    return 1.0 / ((double)i * (i + 1));
}


double sum(int n) {

    double total = 0;

    for (int i=1; i<=n; i++) {
        total += fn(i);
    }

    return total;

}

int main() {

    int num_threads = 1;
    int n = 1000000000;

    double start_time = omp_get_wtime();
    double result = sum(n);
    double end_time = omp_get_wtime();

    printf("time: %fseconds\n", end_time - start_time);
    printf("%f", result);

    log_execution_time("ex1.csv", "sequential", n, num_threads, end_time - start_time);

    return 0;
}