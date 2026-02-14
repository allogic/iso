__forceinline float deg_to_rad(float a) {
  return a * DEG_TO_RAD;
}
__forceinline float rad_to_deg(float a) {
  return a * RAD_TO_DEG;
}

__forceinline float clampf(float a, float min, float max) {
  return fmaxf(min, fminf(max, a));
}
__forceinline int32_t clampi(int32_t a, int32_t min, int32_t max) {
  return (int32_t)fmaxf((float)min, fminf((float)max, (float)a));
}

__forceinline float fsignf(float a) {
  return (a > 0.0F) - (a < 0.0F);
}

__forceinline int32_t vec_to_index(ivector3_t position, ivector3_t size) {
  return (position.x) +
         (position.y * size.x) +
         (position.z * size.x * size.y);
}
__forceinline ivector3_t index_to_vec(int32_t index, ivector3_t size) {
  return (ivector3_t){
    index % size.x,
    (index / size.x) % size.y,
    index / (size.x * size.y),
  };
}
