__forceinline vector4_t vector4_zero(void) {
  vector4_t v = {
    0.0F,
    0.0F,
    0.0F,
    0.0F,
  };

  return v;
}
__forceinline vector4_t vector4_one(void) {
  vector4_t v = {
    1.0F,
    1.0F,
    1.0F,
    1.0F,
  };

  return v;
}
__forceinline vector4_t vector4_xyzw(float x, float y, float z, float w) {
  vector4_t v = {
    x,
    y,
    z,
    w,
  };

  return v;
}
__forceinline vector4_t vector4_negate(vector4_t a) {
  vector4_t v = {
    -a.x,
    -a.y,
    -a.z,
    -a.w,
  };

  return v;
}
__forceinline vector4_t vector4_add(vector4_t a, vector4_t b) {
  vector4_t v = {
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
    a.w + b.w,
  };

  return v;
}
__forceinline vector4_t vector4_sub(vector4_t a, vector4_t b) {
  vector4_t v = {
    a.x - b.x,
    a.y - b.y,
    a.z - b.z,
    a.w - b.w,
  };

  return v;
}
__forceinline vector4_t vector4_mul(vector4_t a, vector4_t b) {
  vector4_t v = {
    a.x * b.x,
    a.y * b.y,
    a.z * b.z,
    a.w * b.w,
  };

  return v;
}
__forceinline vector4_t vector4_div(vector4_t a, vector4_t b) {
  vector4_t v = {
    a.x / b.x,
    a.y / b.y,
    a.z / b.z,
    a.w / b.w,
  };

  return v;
}
__forceinline vector4_t vector4_adds(vector4_t a, float b) {
  vector4_t v = {
    a.x + b,
    a.y + b,
    a.z + b,
    a.w + b,
  };

  return v;
}
__forceinline vector4_t vector4_subs(vector4_t a, float b) {
  vector4_t v = {
    a.x - b,
    a.y - b,
    a.z - b,
    a.w - b,
  };

  return v;
}
__forceinline vector4_t vector4_muls(vector4_t a, float b) {
  vector4_t v = {
    a.x * b,
    a.y * b,
    a.z * b,
    a.w * b,
  };

  return v;
}
__forceinline vector4_t vector4_divs(vector4_t a, float b) {
  vector4_t v = {
    a.x / b,
    a.y / b,
    a.z / b,
    a.w / b,
  };

  return v;
}
__forceinline vector4_t vector4_norm(vector4_t a) {
  float l = vector4_length(a);

  if (l > 0.0F) {
    return vector4_muls(a, 1.0F / l);
  } else {
    return vector4_zero();
  }
}
__forceinline vector4_t vector4_inv(vector4_t a) {
  vector4_t v = {
    1.0F / a.x,
    1.0F / a.y,
    1.0F / a.z,
    1.0F / a.w,
  };

  return v;
}
__forceinline vector4_t vector4_floor(vector4_t a) {
  vector4_t v = {
    floor(a.x),
    floor(a.y),
    floor(a.z),
    floor(a.w),
  };

  return v;
}
__forceinline vector4_t vector4_ceil(vector4_t a) {
  vector4_t v = {
    ceil(a.x),
    ceil(a.y),
    ceil(a.z),
    ceil(a.w),
  };

  return v;
}
__forceinline float vector4_dot(vector4_t a, vector4_t b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}
__forceinline float vector4_length(vector4_t a) {
  return sqrtf(vector4_dot(a, a));
}
__forceinline float vector4_length2(vector4_t a) {
  return vector4_dot(a, a);
}
__forceinline void vector4_print(vector4_t a) {
  printf("[%f, %f, %f, %f]\n", a.x, a.y, a.z, a.w);
}
