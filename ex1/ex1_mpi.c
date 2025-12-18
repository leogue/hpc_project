#include <mpi.h>
#include <stdio.h>
#include "../utils/utils.h"

double fn(int i) {
    return 1.0 / ((double)i * (i + 1));
}

double sum(int n, int rank, int size) {
    double local_sum = 0.0;

    for (int i = rank + 1; i <= n; i += size) {
        local_sum += fn(i);
    }

    return local_sum;
}

int main(int argc, char** argv) {
    int rank, size;
    int n = 1000000000;
    double local_sum, total_sum = 0.0;
    double start_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    start_time = MPI_Wtime();

    local_sum = sum(n, rank, size);

    MPI_Reduce(&local_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    end_time = MPI_Wtime();

    if (rank == 0) {
        printf("time: %f seconds\n", end_time - start_time);
        printf("%f\n", total_sum);

        log_execution_time("ex1.csv", "mpi", n, size, end_time - start_time);
    }

    MPI_Finalize();

    return 0;
}