#include "3DPoint.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (size < 3) {
    if (rank == 0) {
      fprintf(
          stderr,
          "This program requires at least 3 processes (Rank 0 and Rank 2).\n");
    }
    MPI_Finalize();
    return 1;
  }

  int n_points = 10; // Number of points to generate

  // Create MPI Datatype for Point3D
  MPI_Datatype mpi_point_type;
  MPI_Type_contiguous(3, MPI_DOUBLE, &mpi_point_type);
  MPI_Type_commit(&mpi_point_type);

  if (rank == 2) {
    // Rank 2 generates points
    srand(time(NULL) + rank); // Seed random
    printf("Rank 2: Generating %d 3D points...\n", n_points);
    Point3D *points = generate_points(n_points);

    // Send to Rank 0
    printf("Rank 2: Sending points to Rank 0...\n");
    MPI_Send(points, n_points, mpi_point_type, 0, 0, MPI_COMM_WORLD);

    free(points);
  } else if (rank == 0) {
    // Rank 0 receives points
    Point3D *received_points = (Point3D *)malloc(n_points * sizeof(Point3D));

    printf("Rank 0: Waiting for points from Rank 2...\n");
    MPI_Recv(received_points, n_points, mpi_point_type, 2, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);

    printf("Rank 0: Received points:\n");
    print_points(received_points, n_points);

    free(received_points);
  }

  MPI_Type_free(&mpi_point_type);
  MPI_Finalize();
  return 0;
}
