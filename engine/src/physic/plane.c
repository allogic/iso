#include <pch.h>

int8_t plane_intersect_plane3(plane_t p1, plane_t p2, plane_t p3, vector3_t *intersection_point) {
  vector3_t n1 = {p1.nx, p1.ny, p1.nz};
  vector3_t n2 = {p2.nx, p2.ny, p2.nz};
  vector3_t n3 = {p3.nx, p3.ny, p3.nz};

  vector3_t n2xn3 = vector3_cross(n2, n3);
  vector3_t n3xn1 = vector3_cross(n3, n1);
  vector3_t n1xn2 = vector3_cross(n1, n2);

  float denom = vector3_dot(n1, n2xn3);

  if (fabsf(denom) < EPSILON_6) {
    return 0;
  }

  intersection_point->x = (-p1.offset * n2xn3.x - p2.offset * n3xn1.x - p3.offset * n1xn2.x) / denom;
  intersection_point->y = (-p1.offset * n2xn3.y - p2.offset * n3xn1.y - p3.offset * n1xn2.y) / denom;
  intersection_point->z = (-p1.offset * n2xn3.z - p2.offset * n3xn1.z - p3.offset * n1xn2.z) / denom;

  return 1;
}
