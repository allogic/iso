#include <pch.h>

int8_t ray_intersect_aabb(ray_t ray, aabb_t aabb, float *t_enter, float *t_exit) {
  float t_min = -FLT_MAX;
  float t_max = FLT_MAX;

  if (fabsf(ray.direction.x) < EPSILON_6) {

    if (ray.origin.x < aabb.min.x || ray.origin.x > aabb.max.x) {
      return 0;
    }

  } else {

    float ray_direction_inv = 1.0F / ray.direction.x;
    float t1 = (aabb.min.x - ray.origin.x) * ray_direction_inv;
    float t2 = (aabb.max.x - ray.origin.x) * ray_direction_inv;

    if (t1 > t2) {
      float tmp = t1;
      t1 = t2;
      t2 = tmp;
    }

    if (t1 > t_min) {
      t_min = t1;
    }

    if (t2 < t_max) {
      t_max = t2;
    }

    if (t_min > t_max) {
      return 0;
    }
  }

  if (fabsf(ray.direction.y) < EPSILON_6) {

    if (ray.origin.y < aabb.min.y || ray.origin.y > aabb.max.y) {
      return 0;
    }

  } else {

    float ray_direction_inv = 1.0F / ray.direction.y;
    float t1 = (aabb.min.y - ray.origin.y) * ray_direction_inv;
    float t2 = (aabb.max.y - ray.origin.y) * ray_direction_inv;

    if (t1 > t2) {
      float tmp = t1;
      t1 = t2;
      t2 = tmp;
    }

    if (t1 > t_min) {
      t_min = t1;
    }

    if (t2 < t_max) {
      t_max = t2;
    }

    if (t_min > t_max) {
      return 0;
    }
  }

  if (fabsf(ray.direction.z) < EPSILON_6) {

    if (ray.origin.z < aabb.min.z || ray.origin.z > aabb.max.z) {
      return 0;
    }

  } else {

    float ray_direction_inv = 1.0F / ray.direction.z;
    float t1 = (aabb.min.z - ray.origin.z) * ray_direction_inv;
    float t2 = (aabb.max.z - ray.origin.z) * ray_direction_inv;

    if (t1 > t2) {
      float tmp = t1;
      t1 = t2;
      t2 = tmp;
    }

    if (t1 > t_min) {
      t_min = t1;
    }

    if (t2 < t_max) {
      t_max = t2;
    }

    if (t_min > t_max) {
      return 0;
    }
  }

  *t_enter = t_min;
  *t_exit = t_max;

  return 1;
}
