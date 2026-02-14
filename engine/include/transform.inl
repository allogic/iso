__forceinline vector3_t transform_local_right(transform_t const *transform) {
  return quaternion_right(transform->local_rotation);
}
__forceinline vector3_t transform_local_up(transform_t const *transform) {
  return quaternion_up(transform->local_rotation);
}
__forceinline vector3_t transform_local_front(transform_t const *transform) {
  return quaternion_front(transform->local_rotation);
}
__forceinline vector3_t transform_local_left(transform_t const *transform) {
  return vector3_negate(quaternion_right(transform->local_rotation));
}
__forceinline vector3_t transform_local_down(transform_t const *transform) {
  return vector3_negate(quaternion_up(transform->local_rotation));
}
__forceinline vector3_t transform_local_back(transform_t const *transform) {
  return vector3_negate(quaternion_front(transform->local_rotation));
}

void transform_set_position(transform_t *transform, vector3_t position) {
  transform->local_position = position;
}
void transform_set_position_xyz(transform_t *transform, float x, float y, float z) {
  transform->local_position.x = x;
  transform->local_position.y = y;
  transform->local_position.z = z;
}
void transform_set_relative_position(transform_t *transform, transform_t *reference, vector3_t position) {
  transform->local_position.x = reference->world_position.x + position.x;
  transform->local_position.y = reference->world_position.y + position.y;
  transform->local_position.z = reference->world_position.z + position.z;
}
void transform_set_relative_position_xyz(transform_t *transform, transform_t *reference, float x, float y, float z) {
  transform->local_position.x = reference->world_position.x + x;
  transform->local_position.y = reference->world_position.y + y;
  transform->local_position.z = reference->world_position.z + z;
}

void transform_set_rotation(transform_t *transform, quaternion_t rotation) {
  transform->local_rotation = quaternion_norm(rotation);
}
void transform_set_rotation_xyzw(transform_t *transform, float x, float y, float z, float w) {
  transform->local_rotation = quaternion_norm(quaternion_xyzw(x, y, z, w));
}
void transform_set_relative_rotation(transform_t *transform, transform_t *reference, quaternion_t rotation) {
  transform->local_rotation = quaternion_norm(quaternion_mul(reference->world_rotation, rotation));
}
void transform_set_relative_rotation_xyzw(transform_t *transform, transform_t *reference, float x, float y, float z, float w) {
  transform->local_rotation = quaternion_norm(quaternion_mul(reference->world_rotation, quaternion_xyzw(x, y, z, w)));
}

void transform_set_euler_angles(transform_t *transform, vector3_t euler_angles) {
  quaternion_t qx = quaternion_angle_axis(euler_angles.x, vector3_right());
  quaternion_t qy = quaternion_angle_axis(euler_angles.y, vector3_up());
  quaternion_t qz = quaternion_angle_axis(euler_angles.z, vector3_front());

  transform->local_rotation = quaternion_norm(quaternion_mul(qy, quaternion_mul(qx, qz)));
}
void transform_set_euler_angles_pyr(transform_t *transform, float p, float y, float r) {
  quaternion_t qx = quaternion_angle_axis(p, vector3_right());
  quaternion_t qy = quaternion_angle_axis(y, vector3_up());
  quaternion_t qz = quaternion_angle_axis(r, vector3_front());

  transform->local_rotation = quaternion_norm(quaternion_mul(qy, quaternion_mul(qx, qz)));
}
void transform_set_relative_euler_angles(transform_t *transform, transform_t *reference, vector3_t euler_angles) {
  quaternion_t qx = quaternion_angle_axis(euler_angles.x, transform_local_right(reference));
  quaternion_t qy = quaternion_angle_axis(euler_angles.y, transform_local_up(reference));
  quaternion_t qz = quaternion_angle_axis(euler_angles.z, transform_local_front(reference));

  transform->local_rotation = quaternion_norm(quaternion_mul(transform->local_rotation, quaternion_mul(qy, quaternion_mul(qx, qz))));
}
void transform_set_relative_euler_angles_pyr(transform_t *transform, transform_t *reference, float p, float y, float r) {
  quaternion_t qx = quaternion_angle_axis(p, transform_local_right(reference));
  quaternion_t qy = quaternion_angle_axis(y, transform_local_up(reference));
  quaternion_t qz = quaternion_angle_axis(r, transform_local_front(reference));

  transform->local_rotation = quaternion_norm(quaternion_mul(transform->local_rotation, quaternion_mul(qy, quaternion_mul(qx, qz))));
}

void transform_set_scale(transform_t *transform, vector3_t scale) {
  transform->local_scale = scale;
}
void transform_set_scale_xyz(transform_t *transform, float x, float y, float z) {
  transform->local_scale.x = x;
  transform->local_scale.y = y;
  transform->local_scale.z = z;
}
void transform_set_relative_scale(transform_t *transform, transform_t *reference, vector3_t scale) {
  transform->local_scale.x = reference->world_scale.x * scale.x;
  transform->local_scale.y = reference->world_scale.y * scale.y;
  transform->local_scale.z = reference->world_scale.z * scale.z;
}
void transform_set_relative_scale_xyz(transform_t *transform, transform_t *reference, float x, float y, float z) {
  transform->local_scale.x = reference->world_scale.x * x;
  transform->local_scale.y = reference->world_scale.y * y;
  transform->local_scale.z = reference->world_scale.z * z;
}
