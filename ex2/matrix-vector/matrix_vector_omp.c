#include "../../utils/utils.h"
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int *omp_matrix_vector_multiplication(int **matrix, int *vec, int n,
                                      int num_threads) {

  int *result = malloc(n * sizeof(int));

  omp_set_dynamic(0); // disable automatic thread allocation
  omp_set_num_threads(num_threads);
#pragma omp parallel for
  for (int i = 0; i < n; i++) {
    int sum = 0;
    // Since it's a tridiagonal matrix, only (i-1), i, and (i+1) elements are
    // possibly non-zero
    if (i > 0) {
      sum += matrix[i][i - 1] * vec[i - 1];
    }
    sum += matrix[i][i] * vec[i];
    if (i < n - 1) {
      sum += matrix[i][i + 1] * vec[i + 1];
    }
    result[i] = sum;
  }

  return result;
}

int *omp_matrix_opti_vector_multiplication(TridiagMatrix *matrix, int *vec,
                                           int n, int num_threads) {

  int *result = malloc(n * sizeof(int));

  result[0] = matrix->main[0] * vec[0] + matrix->upper[0] * vec[1];
  result[n - 1] =
      matrix->lower[n - 2] * vec[n - 2] + matrix->main[n - 1] * vec[n - 1];

  omp_set_dynamic(0); // disable automatic thread allocation
  omp_set_num_threads(num_threads);
#pragma omp parallel for
  for (int i = 1; i < n - 1; i++) {
    result[i] = matrix->lower[i - 1] * vec[i - 1] + matrix->main[i] * vec[i] +
                matrix->upper[i] * vec[i + 1];
  }

  return result;
}

int main() {

  init_random();

  int num_threads = 8;

  // ################################################################################
  // Naive method
  // ################################################################################
  //
  // int n = 10000;
  //
  // int* vec = random_vec(n);
  // int** matrix = random_tridiagonal_matrix(n);
  //
  // double start_time = omp_get_wtime();
  // int* result = omp_matrix_vector_multiplication(matrix, vec, n,
  // num_threads); double end_time = omp_get_wtime();
  //
  // printf("OpenMP matrix vector multiplication with %d threads time: %fseconds\n", num_threads, end_time - start_time);
  //
  // log_execution_time("matrix_vector_naive.csv", "omp", n, num_threads,
  // end_time - start_time);
  //
  //
  // // CLEANUP
  // free(vec);
  // free(result);
  //
  // for (int i = 0; i < n; i++) {
  //     free(matrix[i]);
  // }
  // free(matrix);

  // ################################################################################
  // Optimal method
  // ################################################################################

  int n = 100000000;

  int *vec = random_vec(n);
  TridiagMatrix *matrix = random_opti_tridiagonal_matrix(n);
  double start_time = omp_get_wtime();
  int *result =
      omp_matrix_opti_vector_multiplication(matrix, vec, n, num_threads);
  double end_time = omp_get_wtime();
  printf(
      "OpenMP matrix vector multiplication with %d threads time: %f seconds\n",
      num_threads, end_time - start_time);
  log_execution_time("matrix_vector_opti.csv", "omp", n, num_threads, end_time - start_time);

  free(vec);
  free(result);
  free(matrix->lower);
  free(matrix->main);
  free(matrix->upper);
  free(matrix);

  return 0;
}