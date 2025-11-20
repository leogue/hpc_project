#include "../../utils/utils.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// Main function
int main(int argc, char **argv) {

  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Global parameters
  int n = 100000000;

  // Rank 0 pointers (Global)
  int *vec = NULL;
  TridiagMatrix *matrix = NULL;
  int *result = NULL;

  // Local pointers
  int local_n;
  int *local_vec;
  int *local_main;
  int *local_upper;
  int *local_lower;
  int *local_result;

  // Distribution variables
  int *counts = malloc(size * sizeof(int));
  int *displs = malloc(size * sizeof(int));
  int *displs_lower = malloc(size * sizeof(int));

  // ################################################################################
  // 1. Initialization and Generation (Rank 0 only)
  // ################################################################################
  if (rank == 0) {
    init_random();
    vec = random_vec(n);
    matrix = random_opti_tridiagonal_matrix(n);
    result = malloc(n * sizeof(int));

    // Load Balancing
    int remainder = n % size;
    int sum = 0;
    for (int i = 0; i < size; i++) {
      counts[i] = n / size + (i < remainder ? 1 : 0);
      displs[i] = sum;

      // Optimization Trick:
      // To calculate row 'i', we need lower[i-1].
      // Shift 'lower' reading by -1 for processes > 0.
      // local_lower[0] will contain the value needed for the chunk start.
      if (i == 0)
        displs_lower[i] = 0;
      else
        displs_lower[i] = displs[i] - 1;

      sum += counts[i];
    }
  }

  // ################################################################################
  // 2. Local size distribution
  // ################################################################################
  // Each process needs its size (local_n)
  MPI_Scatter(counts, 1, MPI_INT, &local_n, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // Local allocation
  local_vec = malloc(local_n * sizeof(int));
  local_main = malloc(local_n * sizeof(int));
  local_upper = malloc(local_n * sizeof(int));
  local_lower = malloc(local_n * sizeof(int));
  local_result = malloc(local_n * sizeof(int));

  // ################################################################################
  // 3. Data Distribution (Scatterv)
  // ################################################################################
  double start_time = MPI_Wtime();

  // Send vector X
  MPI_Scatterv(vec, counts, displs, MPI_INT, local_vec, local_n, MPI_INT, 0,
               MPI_COMM_WORLD);

  // Send main diagonal
  MPI_Scatterv(rank == 0 ? matrix->main : NULL, counts, displs, MPI_INT,
               local_main, local_n, MPI_INT, 0, MPI_COMM_WORLD);

  // Send upper diagonal
  MPI_Scatterv(rank == 0 ? matrix->upper : NULL, counts, displs, MPI_INT,
               local_upper, local_n, MPI_INT, 0, MPI_COMM_WORLD);

  // Send lower diagonal (with shift)
  MPI_Scatterv(rank == 0 ? matrix->lower : NULL, counts, displs_lower, MPI_INT,
               local_lower, local_n, MPI_INT, 0, MPI_COMM_WORLD);

  // ################################################################################
  // 4. Halo Exchange (Ghost Cells)
  // ################################################################################
  // Need vec[i-1] (left neighbor) and vec[i+1] (right neighbor)

  int ghost_left = 0;
  int ghost_right = 0;
  MPI_Status status;

  // Exchange with LEFT neighbor (Rank - 1)
  if (rank > 0) {
    MPI_Sendrecv(&local_vec[0], 1, MPI_INT, rank - 1, 0, &ghost_left, 1,
                 MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
  }

  // Exchange with RIGHT neighbor (Rank + 1)
  if (rank < size - 1) {
    MPI_Sendrecv(&local_vec[local_n - 1], 1, MPI_INT, rank + 1, 0, &ghost_right,
                 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &status);
  }

  // ################################################################################
  // 5. Local Calculation
  // ################################################################################

  // Formula: result[i] = lower[i-1]*vec[i-1] + main[i]*vec[i] +
  // upper[i]*vec[i+1]

  for (int i = 0; i < local_n; i++) {
    long long sum = 0; // Long long to avoid overflow

    // 1. Central Term
    sum += local_main[i] * local_vec[i];

    // 2. Left Term (Lower)
    // local_lower[i] matches lower[global_i - 1]
    if (i > 0) {
      sum += local_lower[i] * local_vec[i - 1];
    } else if (rank > 0) {
      // Local block start -> use left ghost cell
      sum += local_lower[0] * ghost_left;
    }
    // Note: If rank==0 and i==0, no left term

    // 3. Right Term (Upper)
    if (i < local_n - 1) {
      sum += local_upper[i] * local_vec[i + 1];
    } else if (rank < size - 1) {
      // Local block end -> use right ghost cell
      sum += local_upper[i] * ghost_right;
    }
    // Note: If rank==last and i==last, no right term

    local_result[i] = (int)sum;
  }

  double end_time = MPI_Wtime();

  // ################################################################################
  // 6. Gather Results
  // ################################################################################

  MPI_Gatherv(local_result, local_n, MPI_INT, result, counts, displs, MPI_INT,
              0, MPI_COMM_WORLD);

  if (rank == 0) {
    printf("MPI Matrix Vector Multiplication with %d processes. Time: %f "
           "seconds\n",
           size, end_time - start_time);

    log_execution_time("matrix_vector_opti.csv", "mpi", n, size,
                       end_time - start_time);

    // Global Cleanup
    free(vec);
    free(result);
    free(matrix->lower);
    free(matrix->main);
    free(matrix->upper);
    free(matrix);
  }

  // Local Cleanup
  free(counts);
  free(displs);
  free(displs_lower);
  free(local_vec);
  free(local_main);
  free(local_upper);
  free(local_lower);
  free(local_result);

  MPI_Finalize();
  return 0;
}