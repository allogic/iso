__forceinline ivector3_t ivector3_zero(void) {
  ivector3_t v = {
    0,
    0,
    0,
  };

  return v;
}
__forceinline ivector3_t ivector3_one(void) {
  ivector3_t v = {
    1,
    1,
    1,
  };

  return v;
}
__forceinline ivector3_t ivector3_right(void) {
  ivector3_t v = {
    1,
    0,
    0,
  };

  return v;
}
__forceinline ivector3_t ivector3_up(void) {
  ivector3_t v = {
    0,
    1,
    0,
  };

  return v;
}
__forceinline ivector3_t ivector3_front(void) {
  ivector3_t v = {
    0,
    0,
    1,
  };

  return v;
}
__forceinline ivector3_t ivector3_left(void) {
  ivector3_t v = {
    -1,
    0,
    0,
  };

  return v;
}
__forceinline ivector3_t ivector3_down(void) {
  ivector3_t v = {
    0,
    -1,
    0,
  };

  return v;
}
__forceinline ivector3_t ivector3_back(void) {
  ivector3_t v = {
    0,
    0,
    -1,
  };

  return v;
}
__forceinline ivector3_t ivector3_xyz(int32_t x, int32_t y, int32_t z) {
  ivector3_t v = {
    x,
    y,
    z,
  };

  return v;
}
__forceinline ivector3_t ivector3_negate(ivector3_t a) {
  ivector3_t v = {
    -a.x,
    -a.y,
    -a.z,
  };

  return v;
}
__forceinline ivector3_t ivector3_add(ivector3_t a, ivector3_t b) {
  ivector3_t v = {
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
  };

  return v;
}
__forceinline ivector3_t ivector3_sub(ivector3_t a, ivector3_t b) {
  ivector3_t v = {
    a.x - b.x,
    a.y - b.y,
    a.z - b.z,
  };

  return v;
}
__forceinline ivector3_t ivector3_mul(ivector3_t a, ivector3_t b) {
  ivector3_t v = {
    a.x * b.x,
    a.y * b.y,
    a.z * b.z,
  };

  return v;
}
__forceinline ivector3_t ivector3_div(ivector3_t a, ivector3_t b) {
  ivector3_t v = {
    a.x / b.x,
    a.y / b.y,
    a.z / b.z,
  };

  return v;
}
__forceinline ivector3_t ivector3_adds(ivector3_t a, int32_t b) {
  ivector3_t v = {
    a.x + b,
    a.y + b,
    a.z + b,
  };

  return v;
}
__forceinline ivector3_t ivector3_subs(ivector3_t a, int32_t b) {
  ivector3_t v = {
    a.x - b,
    a.y - b,
    a.z - b,
  };

  return v;
}
__forceinline ivector3_t ivector3_muls(ivector3_t a, int32_t b) {
  ivector3_t v = {
    a.x * b,
    a.y * b,
    a.z * b,
  };

  return v;
}
__forceinline ivector3_t ivector3_divs(ivector3_t a, int32_t b) {
  ivector3_t v = {
    a.x / b,
    a.y / b,
    a.z / b,
  };

  return v;
}
__forceinline float ivector3_dot(ivector3_t a, ivector3_t b) {
  return (float)((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
}
__forceinline float ivector3_length(ivector3_t a) {
  return sqrtf(ivector3_dot(a, a));
}
__forceinline float ivector3_length2(ivector3_t a) {
  return ivector3_dot(a, a);
}
__forceinline void ivector3_print(ivector3_t a) {
  printf("[%d, %d, %d]\n", a.x, a.y, a.z);
}
