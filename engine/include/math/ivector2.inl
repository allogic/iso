__forceinline ivector2_t ivector2_zero(void) {
  ivector2_t v = {
    0,
    0,
  };

  return v;
}
__forceinline ivector2_t ivector2_one(void) {
  ivector2_t v = {
    1,
    1,
  };

  return v;
}
__forceinline ivector2_t ivector2_right(void) {
  ivector2_t v = {
    1,
    0,
  };

  return v;
}
__forceinline ivector2_t ivector2_up(void) {
  ivector2_t v = {
    0,
    1,
  };

  return v;
}
__forceinline ivector2_t ivector2_left(void) {
  ivector2_t v = {
    -1,
    0,
  };

  return v;
}
__forceinline ivector2_t ivector2_down(void) {
  ivector2_t v = {
    0,
    -1,
  };

  return v;
}
__forceinline ivector2_t ivector2_xy(int32_t x, int32_t y) {
  ivector2_t v = {
    x,
    y,
  };

  return v;
}
__forceinline ivector2_t ivector2_negate(ivector2_t a) {
  ivector2_t v = {
    -a.x,
    -a.y,
  };

  return v;
}
__forceinline ivector2_t ivector2_add(ivector2_t a, ivector2_t b) {
  ivector2_t v = {
    a.x + b.x,
    a.y + b.y,
  };

  return v;
}
__forceinline ivector2_t ivector2_sub(ivector2_t a, ivector2_t b) {
  ivector2_t v = {
    a.x - b.x,
    a.y - b.y,
  };

  return v;
}
__forceinline ivector2_t ivector2_mul(ivector2_t a, ivector2_t b) {
  ivector2_t v = {
    a.x * b.x,
    a.y * b.y,
  };

  return v;
}
__forceinline ivector2_t ivector2_div(ivector2_t a, ivector2_t b) {
  ivector2_t v = {
    a.x / b.x,
    a.y / b.y,
  };

  return v;
}
__forceinline ivector2_t ivector2_adds(ivector2_t a, int32_t b) {
  ivector2_t v = {
    a.x + b,
    a.y + b,
  };

  return v;
}
__forceinline ivector2_t ivector2_subs(ivector2_t a, int32_t b) {
  ivector2_t v = {
    a.x - b,
    a.y - b,
  };

  return v;
}
__forceinline ivector2_t ivector2_muls(ivector2_t a, int32_t b) {
  ivector2_t v = {
    a.x * b,
    a.y * b,
  };

  return v;
}
__forceinline ivector2_t ivector2_divs(ivector2_t a, int32_t b) {
  ivector2_t v = {
    a.x / b,
    a.y / b,
  };

  return v;
}
__forceinline float ivector2_dot(ivector2_t a, ivector2_t b) {
  return (float)((a.x * b.x) + (a.y * b.y));
}
__forceinline float ivector2_length(ivector2_t a) {
  return sqrtf(ivector2_dot(a, a));
}
__forceinline float ivector2_length2(ivector2_t a) {
  return ivector2_dot(a, a);
}
__forceinline void ivector2_print(ivector2_t a) {
  printf("[%d, %d]\n", a.x, a.y);
}
