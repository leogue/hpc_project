#ifndef UTILS_H
#define UTILS_H


typedef struct {
    int n;
    int *lower; // sub diagonal
    int *main;  // diagonal
    int *upper; // super diagonal
} TridiagMatrix;

void init_random(void);

void print_vec(int* vec, int n);

void print_matrix(int** matrix, int n);

int* random_vec(int n);

int** random_tridiagonal_matrix(int n);

TridiagMatrix* random_opti_tridiagonal_matrix(int n );


#endif