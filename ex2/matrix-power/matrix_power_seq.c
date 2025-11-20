#include "../../utils/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Helper to get current time in seconds
double get_time() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec + ts.tv_nsec * 1e-9;
}

// Compute A^2 (Pentadiagonal) from A (Tridiagonal)
PentaDiagMatrix *compute_square_tridiagonal(TridiagMatrix *A) {
  int n = A->n;
  PentaDiagMatrix *R = malloc(sizeof(PentaDiagMatrix));
  R->n = n;
  R->main = malloc(n * sizeof(int));
  R->upper1 = malloc((n - 1) * sizeof(int));
  R->upper2 = malloc((n - 2) * sizeof(int));
  R->lower1 = malloc((n - 1) * sizeof(int));
  R->lower2 = malloc((n - 2) * sizeof(int));

  // Access helpers (handling boundaries with 0)
  // A->main[i] is valid for 0 <= i < n
  // A->upper[i] is valid for 0 <= i < n-1 (corresponds to A_{i, i+1})
  // A->lower[i] is valid for 0 <= i < n-1 (corresponds to A_{i+1, i})

  // Notation:
  // M[i] = A_{i,i}
  // U[i] = A_{i, i+1}
  // L[i] = A_{i+1, i}  => Note: A->lower[i] is A_{i+1, i}. Be careful with
  // indices. Usually lower[i] means element at row i+1, col i.

  int *M = A->main;
  int *U = A->upper;
  int *L = A->lower;

  for (int i = 0; i < n; i++) {
    // 1. Main Diagonal (R_{i,i})
    // sum_k A_{ik} A_{ki}
    // k = i-1: A_{i, i-1} * A_{i-1, i} = L[i-1] * U[i-1] (if i>0)
    // k = i:   A_{i, i}   * A_{i, i}   = M[i]   * M[i]
    // k = i+1: A_{i, i+1} * A_{i+1, i} = U[i]   * L[i]   (if i<n-1)

    int val = M[i] * M[i];
    if (i > 0)
      val += L[i - 1] * U[i - 1];
    if (i < n - 1)
      val += U[i] * L[i];
    R->main[i] = val;

    // 2. Upper1 Diagonal (R_{i, i+1})
    // sum_k A_{ik} A_{k, i+1}
    // k = i:   A_{i, i}   * A_{i, i+1}   = M[i] * U[i]
    // k = i+1: A_{i, i+1} * A_{i+1, i+1} = U[i] * M[i+1]
    if (i < n - 1) {
      R->upper1[i] = M[i] * U[i] + U[i] * M[i + 1];
    }

    // 3. Upper2 Diagonal (R_{i, i+2})
    // sum_k A_{ik} A_{k, i+2}
    // k = i+1: A_{i, i+1} * A_{i+1, i+2} = U[i] * U[i+1]
    if (i < n - 2) {
      R->upper2[i] = U[i] * U[i + 1];
    }

    // 4. Lower1 Diagonal (R_{i+1, i})
    // sum_k A_{i+1, k} A_{k, i}
    // k = i:   A_{i+1, i} * A_{i, i}   = L[i] * M[i]
    // k = i+1: A_{i+1, i+1} * A_{i+1, i} = M[i+1] * L[i]
    if (i < n - 1) {
      R->lower1[i] = L[i] * M[i] + M[i + 1] * L[i];
    }

    // 5. Lower2 Diagonal (R_{i+2, i})
    // sum_k A_{i+2, k} A_{k, i}
    // k = i+1: A_{i+2, i+1} * A_{i+1, i} = L[i+1] * L[i]
    if (i < n - 2) {
      R->lower2[i] = L[i + 1] * L[i];
    }
  }

  return R;
}

// Compute A^3 (Heptadiagonal) from A (Tridiagonal) and A^2 (Pentadiagonal)
// A^3 = A * A^2
HeptaDiagMatrix *compute_cube_tridiagonal(TridiagMatrix *A,
                                          PentaDiagMatrix *A2) {
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

  for (int i = 0; i < n; i++) {
    // A_{ik} is non-zero for k in {i-1, i, i+1}
    // (A^3)_{ij} = L[i-1]*(A^2)_{i-1, j} + M[i]*(A^2)_{i, j} + U[i]*(A^2)_{i+1,
    // j} Be careful with boundaries.

    // 1. Main Diagonal (j=i)
    long long val = 0;
    if (i > 0)
      val += (long long)L[i - 1] *
             U1_2[i - 1]; // A_{i, i-1} * (A^2)_{i-1, i} = L[i-1] * U1_2[i-1]
    val += (long long)M[i] * M2[i]; // A_{i, i}   * (A^2)_{i, i}
    if (i < n - 1)
      val += (long long)U[i] *
             L1_2[i]; // A_{i, i+1} * (A^2)_{i+1, i} = U[i] * L1_2[i]
    R->main[i] = (int)val;

    // 2. Upper1 (j=i+1)
    if (i < n - 1) {
      long long v = 0;
      if (i > 0)
        v += (long long)L[i - 1] *
             U2_2[i - 1]; // A_{i, i-1} * (A^2)_{i-1, i+1} = L[i-1] * U2_2[i-1]
      v += (long long)M[i] * U1_2[i];   // A_{i, i} * (A^2)_{i, i+1}
      v += (long long)U[i] * M2[i + 1]; // A_{i, i+1} * (A^2)_{i+1, i+1}
      R->upper1[i] = (int)v;
    }

    // 3. Upper2 (j=i+2)
    if (i < n - 2) {
      long long v = 0;
      // k=i-1: (A^2)_{i-1, i+2} is 0 (dist 3)
      v += (long long)M[i] * U2_2[i];     // A_{i, i} * (A^2)_{i, i+2}
      v += (long long)U[i] * U1_2[i + 1]; // A_{i, i+1} * (A^2)_{i+1, i+2}
      R->upper2[i] = (int)v;
    }

    // 4. Upper3 (j=i+3)
    if (i < n - 3) {
      long long v = 0;
      v += (long long)U[i] * U2_2[i + 1]; // A_{i, i+1} * (A^2)_{i+1, i+3}
      R->upper3[i] = (int)v;
    }

    // 5. Lower1 (j=i-1) (row i, col i-1)
    if (i > 0) {
      long long v = 0;
      v += (long long)L[i - 1] * M2[i - 1]; // A_{i, i-1} * (A^2)_{i-1, i-1}
      v += (long long)M[i] * L1_2[i - 1];   // A_{i, i} * (A^2)_{i, i-1}
      if (i < n - 1)
        v += (long long)U[i] *
             L2_2[i - 1]; // A_{i, i+1} * (A^2)_{i+1, i-1} = U[i] * L2_2[i-1]
      R->lower1[i - 1] = (int)v;
    }

    // 6. Lower2 (j=i-2) (row i, col i-2)
    if (i > 1) {
      long long v = 0;
      v += (long long)L[i - 1] * L1_2[i - 2]; // A_{i, i-1} * (A^2)_{i-1, i-2}
      v += (long long)M[i] * L2_2[i - 2];     // A_{i, i} * (A^2)_{i, i-2}
      R->lower2[i - 2] = (int)v;
    }

    // 7. Lower3 (j=i-3)
    if (i > 2) {
      long long v = 0;
      v += (long long)L[i - 1] * L2_2[i - 3]; // A_{i, i-1} * (A^2)_{i-1, i-3}
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

  int n = 100000000; // 100 Million
  printf("Generating Tridiagonal Matrix of size %d...\n", n);
  TridiagMatrix *A = random_opti_tridiagonal_matrix(n);

  printf("Computing A^2 (Sequential)...\n");
  double start = get_time();
  PentaDiagMatrix *A2 = compute_square_tridiagonal(A);
  double end = get_time();
  printf("A^2 computed in %f seconds.\n", end - start);
  log_execution_time("matrix_power2_seq.csv", "sequential", n, 1, end - start);

  printf("Computing A^3 (Sequential)...\n");
  start = get_time();
  HeptaDiagMatrix *A3 = compute_cube_tridiagonal(A, A2);
  end = get_time();
  printf("A^3 computed in %f seconds.\n", end - start);
  log_execution_time("matrix_power3_seq.csv", "sequential", n, 1, end - start);

  // Cleanup
  free(A->main);
  free(A->upper);
  free(A->lower);
  free(A);
  free_penta(A2);
  free_hepta(A3);

  return 0;
}
