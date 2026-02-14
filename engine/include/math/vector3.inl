__forceinline vector3_t vector3_zero(void) {
  vector3_t v = {
    0.0F,
    0.0F,
    0.0F,
  };

  return v;
}
__forceinline vector3_t vector3_one(void) {
  vector3_t v = {
    1.0F,
    1.0F,
    1.0F,
  };

  return v;
}
__forceinline vector3_t vector3_right(void) {
  vector3_t v = {
    1.0F,
    0.0F,
    0.0F,
  };

  return v;
}
__forceinline vector3_t vector3_up(void) {
  vector3_t v = {
    0.0F,
    1.0F,
    0.0F,
  };

  return v;
}
__forceinline vector3_t vector3_front(void) {
  vector3_t v = {
    0.0F,
    0.0F,
    1.0F,
  };

  return v;
}
__forceinline vector3_t vector3_left(void) {
  vector3_t v = {
    -1.0F,
    0.0F,
    0.0F,
  };

  return v;
}
__forceinline vector3_t vector3_down(void) {
  vector3_t v = {
    0.0F,
    -1.0F,
    0.0F,
  };

  return v;
}
__forceinline vector3_t vector3_back(void) {
  vector3_t v = {
    0.0F,
    0.0F,
    -1.0F,
  };

  return v;
}
__forceinline vector3_t vector3_xyz(float x, float y, float z) {
  vector3_t v = {
    x,
    y,
    z,
  };

  return v;
}
__forceinline vector3_t vector3_negate(vector3_t a) {
  vector3_t v = {
    -a.x,
    -a.y,
    -a.z,
  };

  return v;
}
__forceinline vector3_t vector3_add(vector3_t a, vector3_t b) {
  vector3_t v = {
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
  };

  return v;
}
__forceinline vector3_t vector3_sub(vector3_t a, vector3_t b) {
  vector3_t v = {
    a.x - b.x,
    a.y - b.y,
    a.z - b.z,
  };

  return v;
}
__forceinline vector3_t vector3_mul(vector3_t a, vector3_t b) {
  vector3_t v = {
    a.x * b.x,
    a.y * b.y,
    a.z * b.z,
  };

  return v;
}
__forceinline vector3_t vector3_div(vector3_t a, vector3_t b) {
  vector3_t v = {
    a.x / b.x,
    a.y / b.y,
    a.z / b.z,
  };

  return v;
}
__forceinline vector3_t vector3_adds(vector3_t a, float b) {
  vector3_t v = {
    a.x + b,
    a.y + b,
    a.z + b,
  };

  return v;
}
__forceinline vector3_t vector3_subs(vector3_t a, float b) {
  vector3_t v = {
    a.x - b,
    a.y - b,
    a.z - b,
  };

  return v;
}
__forceinline vector3_t vector3_muls(vector3_t a, float b) {
  vector3_t v = {
    a.x * b,
    a.y * b,
    a.z * b,
  };

  return v;
}
__forceinline vector3_t vector3_divs(vector3_t a, float b) {
  vector3_t v = {
    a.x / b,
    a.y / b,
    a.z / b,
  };

  return v;
}
__forceinline vector3_t vector3_norm(vector3_t a) {
  float l = vector3_length(a);

  if (l > 0.0F) {
    return vector3_muls(a, 1.0F / l);
  } else {
    return vector3_zero();
  }
}
__forceinline vector3_t vector3_inv(vector3_t a) {
  vector3_t v = {
    1.0F / a.x,
    1.0F / a.y,
    1.0F / a.z,
  };

  return v;
}
__forceinline vector3_t vector3_floor(vector3_t a) {
  vector3_t v = {
    floor(a.x),
    floor(a.y),
    floor(a.z),
  };

  return v;
}
__forceinline vector3_t vector3_ceil(vector3_t a) {
  vector3_t v = {
    ceil(a.x),
    ceil(a.y),
    ceil(a.z),
  };

  return v;
}
__forceinline vector3_t vector3_cross(vector3_t a, vector3_t b) {
  vector3_t v = {
    (a.y * b.z) - (a.z * b.y),
    (a.z * b.x) - (a.x * b.z),
    (a.x * b.y) - (a.y * b.x),
  };

  return v;
}
__forceinline vector3_t vector3_rotate(vector3_t a, quaternion_t b) {
  float xx = b.x * b.x;
  float yy = b.y * b.y;
  float zz = b.z * b.z;

  float xy = b.x * b.y;
  float xz = b.x * b.z;
  float yz = b.y * b.z;

  float wx = b.w * b.x;
  float wy = b.w * b.y;
  float wz = b.w * b.z;

  vector3_t v = {
    (1.0F - 2.0F * (yy + zz)) * a.x + 2.0F * (xy - wz) * a.y + 2.0F * (xz + wy) * a.z,
    2.0F * (xy + wz) * a.x + (1.0F - 2.0F * (xx + zz)) * a.y + 2.0F * (yz - wx) * a.z,
    2.0F * (xz - wy) * a.x + 2.0F * (yz + wx) * a.y + (1.0F - 2.0F * (xx + yy)) * a.z,
  };

  return v;
}
__forceinline float vector3_dot(vector3_t a, vector3_t b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}
__forceinline float vector3_length(vector3_t a) {
  return sqrtf(vector3_dot(a, a));
}
__forceinline float vector3_length2(vector3_t a) {
  return vector3_dot(a, a);
}
__forceinline void vector3_print(vector3_t a) {
  printf("[%f, %f, %f]\n", a.x, a.y, a.z);
}
