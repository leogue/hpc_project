#include "../../utils/utils.h"
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

// Compute A^2 (Pentadiagonal) from A (Tridiagonal)
PentaDiagMatrix *compute_square_tridiagonal_omp(TridiagMatrix *A,
                                                int num_threads) {
  int n = A->n;
  PentaDiagMatrix *R = malloc(sizeof(PentaDiagMatrix));
  R->n = n;
  R->main = malloc(n * sizeof(int));
  R->upper1 = malloc((n - 1) * sizeof(int));
  R->upper2 = malloc((n - 2) * sizeof(int));
  R->lower1 = malloc((n - 1) * sizeof(int));
  R->lower2 = malloc((n - 2) * sizeof(int));

  int *M = A->main;
  int *U = A->upper;
  int *L = A->lower;

  omp_set_dynamic(0);
  omp_set_num_threads(num_threads);
  #pragma omp parallel for
  for (int i = 0; i < n; i++) {
    // 1. Main Diagonal (R_{i,i})
    int val = M[i] * M[i];
    if (i > 0)
      val += L[i - 1] * U[i - 1];
    if (i < n - 1)
      val += U[i] * L[i];
    R->main[i] = val;

    // 2. Upper1 Diagonal (R_{i, i+1})
    if (i < n - 1) {
      R->upper1[i] = M[i] * U[i] + U[i] * M[i + 1];
    }

    // 3. Upper2 Diagonal (R_{i, i+2})
    if (i < n - 2) {
      R->upper2[i] = U[i] * U[i + 1];
    }

    // 4. Lower1 Diagonal (R_{i+1, i})
    if (i < n - 1) {
      R->lower1[i] = L[i] * M[i] + M[i + 1] * L[i];
    }

    // 5. Lower2 Diagonal (R_{i+2, i})
    if (i < n - 2) {
      R->lower2[i] = L[i + 1] * L[i];
    }
  }

  return R;
}

// Compute A^3 (Heptadiagonal) from A (Tridiagonal) and A^2 (Pentadiagonal)
HeptaDiagMatrix *compute_cube_tridiagonal_omp(TridiagMatrix *A,
                                              PentaDiagMatrix *A2,
                                              int num_threads) {
  int n = A->n;
  HeptaDiagMatrix *R = malloc(sizeof(HeptaDiagMatrix));
  R->n = n;
  R->main = malloc(n * sizeof(int));
  R->upper1 = malloc((n - 1) * sizeof(int));
  R->upper2 = malloc((n - 2) * sizeof(int));
  R->upper3 = malloc((n - 3) * sizeof(int));
  R->lower1 = malloc((n - 1) * sizeof(int));
  R->lower2 = malloc((n - 2) * sizeof(int));
  R->lower3 = malloc((n - 3) * sizeof(int));

  int *M = A->main;
  int *U = A->upper;
  int *L = A->lower;

  int *M2 = A2->main;
  int *U1_2 = A2->upper1;
  int *U2_2 = A2->upper2;
  int *L1_2 = A2->lower1;
  int *L2_2 = A2->lower2;

  omp_set_dynamic(0);
  omp_set_num_threads(num_threads);
  #pragma omp parallel for
  for (int i = 0; i < n; i++) {
    // 1. Main Diagonal (j=i)
    long long val = 0;
    if (i > 0)
      val += (long long)L[i - 1] * U1_2[i - 1];
    val += (long long)M[i] * M2[i];
    if (i < n - 1)
      val += (long long)U[i] * L1_2[i];
    R->main[i] = (int)val;

    // 2. Upper1 (j=i+1)
    if (i < n - 1) {
      long long v = 0;
      if (i > 0)
        v += (long long)L[i - 1] * U2_2[i - 1];
      v += (long long)M[i] * U1_2[i];
      v += (long long)U[i] * M2[i + 1];
      R->upper1[i] = (int)v;
    }

    // 3. Upper2 (j=i+2)
    if (i < n - 2) {
      long long v = 0;
      v += (long long)M[i] * U2_2[i];
      v += (long long)U[i] * U1_2[i + 1];
      R->upper2[i] = (int)v;
    }

    // 4. Upper3 (j=i+3)
    if (i < n - 3) {
      long long v = 0;
      v += (long long)U[i] * U2_2[i + 1];
      R->upper3[i] = (int)v;
    }

    // 5. Lower1 (j=i-1)
    if (i > 0) {
      long long v = 0;
      v += (long long)L[i - 1] * M2[i - 1];
      v += (long long)M[i] * L1_2[i - 1];
      if (i < n - 1)
        v += (long long)U[i] * L2_2[i - 1];
      R->lower1[i - 1] = (int)v;
    }

    // 6. Lower2 (j=i-2)
    if (i > 1) {
      long long v = 0;
      v += (long long)L[i - 1] * L1_2[i - 2];
      v += (long long)M[i] * L2_2[i - 2];
      R->lower2[i - 2] = (int)v;
    }

    // 7. Lower3 (j=i-3)
    if (i > 2) {
      long long v = 0;
      v += (long long)L[i - 1] * L2_2[i - 3];
      R->lower3[i - 3] = (int)v;
    }
  }

  return R;
}

void free_penta(PentaDiagMatrix *m) {
  if (!m)
    return;
  free(m->main);
  free(m->upper1);
  free(m->upper2);
  free(m->lower1);
  free(m->lower2);
  free(m);
}

void free_hepta(HeptaDiagMatrix *m) {
  if (!m)
    return;
  free(m->main);
  free(m->upper1);
  free(m->upper2);
  free(m->upper3);
  free(m->lower1);
  free(m->lower2);
  free(m->lower3);
  free(m);
}

int main() {
  init_random();

  int n = 100000000;   // 100 Million
  int num_threads = 8; // Default to 8 threads

  printf("Generating Tridiagonal Matrix of size %d...\n", n);
  TridiagMatrix *A = random_opti_tridiagonal_matrix(n);

  printf("Computing A^2 (OpenMP with %d threads)...\n", num_threads);
  double start = omp_get_wtime();
  PentaDiagMatrix *A2 = compute_square_tridiagonal_omp(A, num_threads);
  double end = omp_get_wtime();
  printf("A^2 computed in %f seconds.\n", end - start);
  log_execution_time("matrix_power2_omp.csv", "omp", n, num_threads,
                     end - start);

  printf("Computing A^3 (OpenMP with %d threads)...\n", num_threads);
  start = omp_get_wtime();
  HeptaDiagMatrix *A3 = compute_cube_tridiagonal_omp(A, A2, num_threads);
  end = omp_get_wtime();
  printf("A^3 computed in %f seconds.\n", end - start);
  log_execution_time("matrix_power3_omp.csv", "omp", n, num_threads,
                     end - start);

  // Cleanup
  free(A->main);
  free(A->upper);
  free(A->lower);
  free(A);
  free_penta(A2);
  free_hepta(A3);

  return 0;
}
