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
      int r = (rand() % 201) - 100; // Random number between -100 and 100
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
      matrix[i][i - 1] = (rand() % 201) - 100;
    matrix[i][i] = (rand() % 201) - 100;
    if (i < n - 1)
      matrix[i][i + 1] = (rand() % 201) - 100;
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
