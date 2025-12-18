#include <omp.h>
#include <stdio.h>
#include "../utils/utils.h"

double fn(int i) {
    return 1.0 / ((double)i * (i + 1));
}


double sum(int n, int nb_threads) {

    double total = 0;


    omp_set_num_threads(nb_threads);
    #pragma omp parallel for reduction(+:total) schedule(static)
    for (int i=1; i<=n; i++) {
        total += fn(i);
    }

    return total;

}

int main() {

    int num_threads = 8;
    int n = 1000000000;

    double start_time = omp_get_wtime();
    double result = sum(n, num_threads);
    double end_time = omp_get_wtime();

    printf("time: %fseconds\n", end_time - start_time);
    printf("%f", result);

    log_execution_time("ex1.csv", "omp", n, num_threads, end_time - start_time);

    return 0;
}