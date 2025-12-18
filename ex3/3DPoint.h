#ifndef POINT3D_H
#define POINT3D_H

typedef struct {
  double x;
  double y;
  double z;
} Point3D;

/**
 * Generates an array of n random 3D points.
 * Returns a pointer to the allocated array.
 */
Point3D *generate_points(int n);

/**
 * Prints the points to stdout.
 */
void print_points(Point3D *points, int n);

#endif