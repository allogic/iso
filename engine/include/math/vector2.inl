__forceinline vector2_t vector2_zero(void) {
  vector2_t v = {
    0.0F,
    0.0F,
  };

  return v;
}
__forceinline vector2_t vector2_one(void) {
  vector2_t v = {
    1.0F,
    1.0F,
  };

  return v;
}
__forceinline vector2_t vector2_right(void) {
  vector2_t v = {
    1.0F,
    0.0F,
  };

  return v;
}
__forceinline vector2_t vector2_up(void) {
  vector2_t v = {
    0.0F,
    1.0F,
  };

  return v;
}
__forceinline vector2_t vector2_left(void) {
  vector2_t v = {
    -1.0F,
    0.0F,
  };

  return v;
}
__forceinline vector2_t vector2_down(void) {
  vector2_t v = {
    0.0F,
    -1.0F,
  };

  return v;
}
__forceinline vector2_t vector2_xy(float x, float y) {
  vector2_t v = {
    x,
    y,
  };

  return v;
}
__forceinline vector2_t vector2_negate(vector2_t a) {
  vector2_t v = {
    -a.x,
    -a.y,
  };

  return v;
}
__forceinline vector2_t vector2_add(vector2_t a, vector2_t b) {
  vector2_t v = {
    a.x + b.x,
    a.y + b.y,
  };

  return v;
}
__forceinline vector2_t vector2_sub(vector2_t a, vector2_t b) {
  vector2_t v = {
    a.x - b.x,
    a.y - b.y,
  };

  return v;
}
__forceinline vector2_t vector2_mul(vector2_t a, vector2_t b) {
  vector2_t v = {
    a.x * b.x,
    a.y * b.y,
  };

  return v;
}
__forceinline vector2_t vector2_div(vector2_t a, vector2_t b) {
  vector2_t v = {
    a.x / b.x,
    a.y / b.y,
  };

  return v;
}
__forceinline vector2_t vector2_adds(vector2_t a, float b) {
  vector2_t v = {
    a.x + b,
    a.y + b,
  };

  return v;
}
__forceinline vector2_t vector2_subs(vector2_t a, float b) {
  vector2_t v = {
    a.x - b,
    a.y - b,
  };

  return v;
}
__forceinline vector2_t vector2_muls(vector2_t a, float b) {
  vector2_t v = {
    a.x * b,
    a.y * b,
  };

  return v;
}
__forceinline vector2_t vector2_divs(vector2_t a, float b) {
  vector2_t v = {
    a.x / b,
    a.y / b,
  };

  return v;
}
__forceinline vector2_t vector2_norm(vector2_t a) {
  float l = vector2_length(a);

  if (l > 0.0F) {
    return vector2_muls(a, 1.0F / l);
  } else {
    return vector2_zero();
  }
}
__forceinline vector2_t vector2_inv(vector2_t a) {
  vector2_t v = {
    1.0F / a.x,
    1.0F / a.y,
  };

  return v;
}
__forceinline vector2_t vector2_floor(vector2_t a) {
  vector2_t v = {
    floor(a.x),
    floor(a.y),
  };

  return v;
}
__forceinline vector2_t vector2_ceil(vector2_t a) {
  vector2_t v = {
    ceil(a.x),
    ceil(a.y),
  };

  return v;
}
__forceinline float vector2_dot(vector2_t a, vector2_t b) {
  return (a.x * b.x) + (a.y * b.y);
}
__forceinline float vector2_length(vector2_t a) {
  return sqrtf(vector2_dot(a, a));
}
__forceinline float vector2_length2(vector2_t a) {
  return vector2_dot(a, a);
}
__forceinline void vector2_print(vector2_t a) {
  printf("[%f, %f]\n", a.x, a.y);
}
