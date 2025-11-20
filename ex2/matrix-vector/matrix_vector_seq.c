#include "../../utils/utils.h"
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int *sequential_matrix_vector_multiplication(int **matrix, int *vec, int n) {

  int *result = malloc(n * sizeof(int));

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

int *sequential_matrix_opti_vector_multiplication(TridiagMatrix *matrix, int *vec, int n) {

  int *result = malloc(n * sizeof(int));

  result[0] = matrix->main[0] * vec[0] + matrix->upper[0] * vec[1];

  for (int i = 1; i < n - 1; i++) {
    result[i] = matrix->lower[i - 1] * vec[i - 1] + matrix->main[i] * vec[i] +
                matrix->upper[i] * vec[i + 1];
  }

  result[n - 1] =
      matrix->lower[n - 2] * vec[n - 2] + matrix->main[n - 1] * vec[n - 1];

  return result;
}

int main() {

  init_random();

  // ################################################################################
  // Naive method
  // ################################################################################

  // int n = 10000;
  //
  // int* vec = random_vec(n);
  // int** matrix = random_tridiagonal_matrix(n);
  //
  // double start_time = omp_get_wtime();
  // int* result = sequential_matrix_vector_multiplication(matrix, vec, n);
  // double end_time = omp_get_wtime();
  // printf("Sequential matrix vector multiplication time: %f seconds\n",
  // end_time - start_time);
  // log_execution_time("matrix_vector_naive.csv", "sequential", n, 1, end_time
  // - start_time);
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
  int *result = sequential_matrix_opti_vector_multiplication(matrix, vec, n);
  double end_time = omp_get_wtime();
  printf("Sequential matrix vector multiplication time: %f seconds\n",
         end_time - start_time);
  log_execution_time("matrix_vector_opti.csv", "sequential", n, 1,
                     end_time - start_time);

  free(vec);
  free(result);
  free(matrix->lower);
  free(matrix->main);
  free(matrix->upper);
  free(matrix);

  return 0;
}