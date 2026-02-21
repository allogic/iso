#ifndef TRANSFORM_H
#define TRANSFORM_H

#define TRANSFORM_MAX_CHILDREN (0x10)

typedef struct transform_t {
  vector3_t local_position;
  quaternion_t local_rotation;
  vector3_t local_scale;
  vector3_t world_position;
  quaternion_t world_rotation;
  vector3_t world_scale;
  struct transform_t *parent;
  struct transform_t *children[TRANSFORM_MAX_CHILDREN];
  int32_t child_count;
} transform_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

matrix4_t transform_matrix(transform_t *transform);

void transform_compute_world_position(transform_t *transform);
void transform_compute_world_rotation(transform_t *transform);
void transform_compute_world_scale(transform_t *transform);

__forceinline vector3_t transform_local_right(transform_t const *transform);
__forceinline vector3_t transform_local_up(transform_t const *transform);
__forceinline vector3_t transform_local_front(transform_t const *transform);
__forceinline vector3_t transform_local_left(transform_t const *transform);
__forceinline vector3_t transform_local_down(transform_t const *transform);
__forceinline vector3_t transform_local_back(transform_t const *transform);

__forceinline void transform_set_position(transform_t *transform, vector3_t position);
__forceinline void transform_set_position_xyz(transform_t *transform, float x, float y, float z);
__forceinline void transform_set_relative_position(transform_t *transform, transform_t *reference, vector3_t position);
__forceinline void transform_set_relative_position_xyz(transform_t *transform, transform_t *reference, float x, float y, float z);

__forceinline void transform_set_rotation(transform_t *transform, quaternion_t rotation);
__forceinline void transform_set_rotation_xyzw(transform_t *transform, float x, float y, float z, float w);
__forceinline void transform_set_relative_rotation(transform_t *transform, transform_t *reference, quaternion_t rotation);
__forceinline void transform_set_relative_rotation_xyzw(transform_t *transform, transform_t *reference, float x, float y, float z, float w);

__forceinline void transform_set_euler_angles(transform_t *transform, vector3_t euler_angles);
__forceinline void transform_set_euler_angles_pyr(transform_t *transform, float p, float y, float r);
__forceinline void transform_set_relative_euler_angles(transform_t *transform, transform_t *reference, vector3_t euler_angles);
__forceinline void transform_set_relative_euler_angles_pyr(transform_t *transform, transform_t *reference, float p, float y, float r);

__forceinline void transform_set_scale(transform_t *transform, vector3_t scale);
__forceinline void transform_set_scale_xyz(transform_t *transform, float x, float y, float z);
__forceinline void transform_set_relative_scale(transform_t *transform, transform_t *reference, vector3_t scale);
__forceinline void transform_set_relative_scale_xyz(transform_t *transform, transform_t *reference, float x, float y, float z);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <transform.inl>

#endif // TRANSFORM_H
