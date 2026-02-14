#ifndef PHYSIC_PLANE_H
#define PHYSIC_PLANE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int8_t plane_intersect_plane3(plane_t p1, plane_t p2, plane_t p3, vector3_t *intersection_point);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PHYSIC_PLANE_H
