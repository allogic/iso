__forceinline ivector4_t ivector4_zero(void) {
  ivector4_t v = {
    0,
    0,
    0,
    0,
  };

  return v;
}
__forceinline ivector4_t ivector4_one(void) {
  ivector4_t v = {
    1,
    1,
    1,
    1,
  };

  return v;
}
__forceinline ivector4_t ivector4_xyzw(int32_t x, int32_t y, int32_t z, int32_t w) {
  ivector4_t v = {
    x,
    y,
    z,
    w,
  };

  return v;
}
__forceinline ivector4_t ivector4_negate(ivector4_t a) {
  ivector4_t v = {
    -a.x,
    -a.y,
    -a.z,
    -a.w,
  };

  return v;
}
__forceinline ivector4_t ivector4_add(ivector4_t a, ivector4_t b) {
  ivector4_t v = {
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
    a.w + b.w,
  };

  return v;
}
__forceinline ivector4_t ivector4_sub(ivector4_t a, ivector4_t b) {
  ivector4_t v = {
    a.x - b.x,
    a.y - b.y,
    a.z - b.z,
    a.w - b.w,
  };

  return v;
}
__forceinline ivector4_t ivector4_mul(ivector4_t a, ivector4_t b) {
  ivector4_t v = {
    a.x * b.x,
    a.y * b.y,
    a.z * b.z,
    a.w * b.w,
  };

  return v;
}
__forceinline ivector4_t ivector4_div(ivector4_t a, ivector4_t b) {
  ivector4_t v = {
    a.x / b.x,
    a.y / b.y,
    a.z / b.z,
    a.w / b.w,
  };

  return v;
}
__forceinline ivector4_t ivector4_adds(ivector4_t a, int32_t b) {
  ivector4_t v = {
    a.x + b,
    a.y + b,
    a.z + b,
    a.w + b,
  };

  return v;
}
__forceinline ivector4_t ivector4_subs(ivector4_t a, int32_t b) {
  ivector4_t v = {
    a.x - b,
    a.y - b,
    a.z - b,
    a.w - b,
  };

  return v;
}
__forceinline ivector4_t ivector4_muls(ivector4_t a, int32_t b) {
  ivector4_t v = {
    a.x * b,
    a.y * b,
    a.z * b,
    a.w * b,
  };

  return v;
}
__forceinline ivector4_t ivector4_divs(ivector4_t a, int32_t b) {
  ivector4_t v = {
    a.x / b,
    a.y / b,
    a.z / b,
    a.w / b,
  };

  return v;
}
__forceinline float ivector4_dot(ivector4_t a, ivector4_t b) {
  return (float)((a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w));
}
__forceinline float ivector4_length(ivector4_t a) {
  return sqrtf(ivector4_dot(a, a));
}
__forceinline float ivector4_length2(ivector4_t a) {
  return ivector4_dot(a, a);
}
__forceinline void ivector4_print(ivector4_t a) {
  printf("[%d, %d, %d, %d]\n", a.x, a.y, a.z, a.w);
}
