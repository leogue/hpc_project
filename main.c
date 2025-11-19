#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <mpi.h>


#include "utils/utils.h"


int* sequential_matrix_vector_multiplication(int** matrix, int* vec, int n) {
    
    int* result = malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        int sum = 0;
        // Since it's a tridiagonal matrix, only (i-1), i, and (i+1) elements are possibly non-zero
        if (i > 0) {
            sum += matrix[i][i-1] * vec[i-1];
        }
        sum += matrix[i][i] * vec[i];
        if (i < n-1) {
            sum += matrix[i][i+1] * vec[i+1];
        }
        result[i] = sum;
    }

    return result;
}

int* openmp_matrix_vector_multiplication(int** matrix, int* vec, int n) {
    
    int* result = malloc(n * sizeof(int));

    omp_set_dynamic(0); // disable automatic thread allocation
    omp_set_num_threads(4);
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        int sum = 0;
        // Since it's a tridiagonal matrix, only (i-1), i, and (i+1) elements are possibly non-zero
        if (i > 0) {
            sum += matrix[i][i-1] * vec[i-1];
        }
        sum += matrix[i][i] * vec[i];
        if (i < n-1) {
            sum += matrix[i][i+1] * vec[i+1];
        }
        result[i] = sum;
    }

    return result;
}

int* mpi_matrix_vector_multiplication(int** matrix, int* vec, int n) {

    MPI_Init(NULL, NULL);
    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int* local_vec = malloc(n / num_procs * sizeof(int));
    MPI_Scatter(vec, n / num_procs, MPI_INT, local_vec, n / num_procs, MPI_INT, 0, MPI_COMM_WORLD);

    int* local_result = malloc(n / num_procs * sizeof(int));
    for (int i = 0; i < n / num_procs; i++) {
        int sum = 0;
        if (i > 0) {
            sum += matrix[i][i-1] * local_vec[i-1];
        }
        sum += matrix[i][i] * local_vec[i];
        if (i < n / num_procs - 1) {
            sum += matrix[i][i+1] * local_vec[i+1];
        }
        local_result[i] = sum;
    }

    int* result = malloc(n * sizeof(int));
    MPI_Gather(local_result, n / num_procs, MPI_INT, result, n / num_procs, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Finalize();
    return result;
}


int main(void) {

    srand(time(NULL)); // Initialize the random number generator

    int n = 5;
    int* vec = random_vec(n);
    int** matrix = random_tridiagonal_matrix(n);
    int* result = sequential_matrix_vector_multiplication(matrix, vec, n);
    int* result_openmp = openmp_matrix_vector_multiplication(matrix, vec, n);

//    print_vec(vec, n);
//    print_matrix(matrix, n);
    print_vec(result, n);
    print_vec(result_openmp, n);

    free(vec);
    free(result);
    free(result_openmp);

    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
    

    return 0;
}