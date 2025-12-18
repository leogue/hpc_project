#include "3DPoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Point3D *generate_points(int n) {
  Point3D *points = (Point3D *)malloc(n * sizeof(Point3D));
  if (points == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }

  // Initialize random number generator (should be done once in main, but for
  // safety here if not done) However, usually srand is called in main. We will
  // assume srand is called in main or we can call it here if n > 0. To avoid
  // resetting seed every time if called multiple times, we'll rely on main
  // calling srand.

  for (int i = 0; i < n; i++) {
    points[i].x = ((double)rand() / RAND_MAX) * 100.0; // 0 to 100
    points[i].y = ((double)rand() / RAND_MAX) * 100.0;
    points[i].z = ((double)rand() / RAND_MAX) * 100.0;
  }

  return points;
}

void print_points(Point3D *points, int n) {
  for (int i = 0; i < n; i++) {
    printf("Point %d: (%.2f, %.2f, %.2f)\n", i, points[i].x, points[i].y,
           points[i].z);
  }
}