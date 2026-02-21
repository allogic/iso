#include <pch.h>

matrix4_t transform_matrix(transform_t *transform) {
  float xx = transform->world_rotation.x * transform->world_rotation.x;
  float yy = transform->world_rotation.y * transform->world_rotation.y;
  float zz = transform->world_rotation.z * transform->world_rotation.z;

  float xy = transform->world_rotation.x * transform->world_rotation.y;
  float xz = transform->world_rotation.x * transform->world_rotation.z;
  float yz = transform->world_rotation.y * transform->world_rotation.z;

  float wx = transform->world_rotation.w * transform->world_rotation.x;
  float wy = transform->world_rotation.w * transform->world_rotation.y;
  float wz = transform->world_rotation.w * transform->world_rotation.z;

  matrix4_t m = {
    (1.0F - 2.0F * (yy + zz)) * transform->world_scale.x,
    2.0F * (xy - wz) * transform->world_scale.y,
    2.0F * (xz + wy) * transform->world_scale.z,
    0.0F,
    2.0F * (xy + wz) * transform->world_scale.x,
    (1.0F - 2.0F * (xx + zz)) * transform->world_scale.y,
    2.0F * (yz - wx) * transform->world_scale.z,
    0.0F,
    2.0F * (xz - wy) * transform->world_scale.x,
    2.0F * (yz + wx) * transform->world_scale.y,
    (1.0F - 2.0F * (xx + yy)) * transform->world_scale.z,
    0.0F,
    transform->world_position.x,
    transform->world_position.y,
    transform->world_position.z,
    1.0F,
  };

  return m;
}

void transform_compute_world_position(transform_t *transform) {
  if (transform->parent) {
    transform->world_position = vector3_add(transform->parent->world_position, vector3_rotate(transform->local_position, transform->parent->world_rotation));
  } else {
    transform->world_position = transform->local_position;
  }

  int32_t child_index = 0;
  int32_t child_count = transform->child_count;

  while (child_index < child_count) {

    transform_compute_world_position(transform->children[child_index]);

    child_index++;
  }
}
void transform_compute_world_rotation(transform_t *transform) {
  if (transform->parent) {
    transform->world_rotation = quaternion_mul(transform->local_rotation, transform->parent->world_rotation);
  } else {
    transform->world_rotation = transform->local_rotation;
  }

  int32_t child_index = 0;
  int32_t child_count = transform->child_count;

  while (child_index < child_count) {

    transform_compute_world_rotation(transform->children[child_index]);

    child_index++;
  }
}
void transform_compute_world_scale(transform_t *transform) {
  if (transform->parent) {
    transform->world_scale = vector3_mul(transform->local_scale, transform->parent->world_scale);
  } else {
    transform->world_scale = transform->local_scale;
  }

  int32_t child_index = 0;
  int32_t child_count = transform->child_count;

  while (child_index < child_count) {

    transform_compute_world_scale(transform->children[child_index]);

    child_index++;
  }
}
