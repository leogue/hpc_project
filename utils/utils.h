#ifndef UTILS_H
#define UTILS_H

typedef struct {
  int n;
  int *lower; // sub diagonal
  int *main;  // diagonal
  int *upper; // super diagonal
} TridiagMatrix;

typedef struct {
  int n;
  int *lower2; // sub-sub diagonal (i-2)
  int *lower1; // sub diagonal (i-1)
  int *main;   // diagonal (i)
  int *upper1; // super diagonal (i+1)
  int *upper2; // super-super diagonal (i+2)
} PentaDiagMatrix;

typedef struct {
  int n;
  int *lower3; // sub-sub-sub diagonal (i-3)
  int *lower2; // sub-sub diagonal (i-2)
  int *lower1; // sub diagonal (i-1)
  int *main;   // diagonal (i)
  int *upper1; // super diagonal (i+1)
  int *upper2; // super-super diagonal (i+2)
  int *upper3; // super-super-super diagonal (i+3)
} HeptaDiagMatrix;

void init_random(void);

void print_vec(int *vec, int n);

void print_matrix(int **matrix, int n);

int *random_vec(int n);

int **random_tridiagonal_matrix(int n);

TridiagMatrix *random_opti_tridiagonal_matrix(int n);

void log_execution_time(const char *filename, const char *method, int size,
                        int nb_process, double time);

#endif