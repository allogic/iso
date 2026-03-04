__forceinline float deg_to_rad(float a) {
  return a * DEG_TO_RAD;
}
__forceinline float rad_to_deg(float a) {
  return a * RAD_TO_DEG;
}

__forceinline double clamp(double a, double min, double max) {
  return fmax(min, fmin(max, a));
}
__forceinline float clampf(float a, float min, float max) {
  return fmaxf(min, fminf(max, a));
}
__forceinline int32_t clampi(int32_t a, int32_t min, int32_t max) {
  if (a < min) {
    return min;
  }

  if (a > max) {
    return max;
  }

  return a;
}
__forceinline uint32_t clampu(uint32_t a, uint32_t min, uint32_t max) {
  if (a < min) {
    return min;
  }

  if (a > max) {
    return max;
  }

  return a;
}

__forceinline float signf(float a) {
  return (a > 0.0F) - (a < 0.0F);
}

__forceinline int32_t floor_div32(float x) {
  int32_t i = (int32_t)x;

  if (x < 0.0f && x != (float)i) {
    i--;
  }

  return i >> 5;
}
