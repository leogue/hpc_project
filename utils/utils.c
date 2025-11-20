#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void init_random(void) { srand(time(NULL)); }

void print_vec(int *vec, int n) {
  for (int i = 0; i < n; i++) {
    printf("%d ", vec[i]);
  }
  printf("\n");
}

void print_matrix(int **matrix, int n) {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      printf("%d ", matrix[i][j]);
    }
    printf("\n");
  }
}

int *random_vec(int n) {
  if (n > 0) {
    int *vec = malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
      int r = (rand() % 21) - 10; // Random number between -10 and 10
      vec[i] = r;
    }

    return vec;
  } else {
    fprintf(stderr, "Error: n must be greater than 0\n");
    exit(1);
  }
}

int **random_tridiagonal_matrix(int n) {

  int **matrix = malloc(n * sizeof(int *));
  for (int i = 0; i < n; i++) {
    matrix[i] = malloc(n * sizeof(int));
  }

  // Initialize the matrix to 0
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      matrix[i][j] = 0;
    }
  }

  // Fill only the non-zero diagonals
  for (int i = 0; i < n; i++) {
    if (i > 0)
      matrix[i][i - 1] = (rand() % 21) - 10;
    matrix[i][i] = (rand() % 21) - 10;
    if (i < n - 1)
      matrix[i][i + 1] = (rand() % 21) - 10;
  }

  return matrix;
}

TridiagMatrix *random_opti_tridiagonal_matrix(int n) {

  if (n <= 0) {
    fprintf(stderr, "Error: n must be greater than 0\n");
    exit(1);
  }

  if (n <= 1) {
    fprintf(stderr, "Error : n must be greater than 1\n");
    exit(1);
  }

  TridiagMatrix *matrix = malloc(sizeof(TridiagMatrix));

  matrix->n = n;
  matrix->lower = random_vec(n - 1);
  matrix->main = random_vec(n);
  matrix->upper = random_vec(n - 1);

  return matrix;
}

void log_execution_time(const char *filename, const char *method, int size, int nb_process, double time) {
  FILE *file = fopen(filename, "r");
  int write_header = 0;
  if (file == NULL) {
    write_header = 1;
  } else {
    // Check if file is empty
    fseek(file, 0, SEEK_END);
    if (ftell(file) == 0) {
      write_header = 1;
    }
    fclose(file);
  }

  file = fopen(filename, "a");
  if (file == NULL) {
    fprintf(stderr, "Error: Could not open file %s for writing\n", filename);
    return;
  }

  if (write_header) {
    fprintf(file, "method,size,nb_proc,time\n");
  }

  fprintf(file, "%s,%d,%d,%lf\n", method, size, nb_process, time);
  fclose(file);
}
