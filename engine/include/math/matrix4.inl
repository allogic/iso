__forceinline matrix4_t matrix4_zero(void) {
  matrix4_t m = {
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
  };

  return m;
}
__forceinline matrix4_t matrix4_identity(void) {
  matrix4_t m = {
    1.0F,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    1.0F,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    1.0F,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    1.0F,
  };

  return m;
}
__forceinline matrix4_t matrix4_inverse(matrix4_t a) {
  float determinant = a.m00 * (a.m11 * (a.m22 * a.m33 - a.m23 * a.m32) - a.m12 * (a.m21 * a.m33 - a.m23 * a.m31) + a.m13 * (a.m21 * a.m32 - a.m22 * a.m31)) -
                      a.m01 * (a.m10 * (a.m22 * a.m33 - a.m23 * a.m32) - a.m12 * (a.m20 * a.m33 - a.m23 * a.m30) + a.m13 * (a.m20 * a.m32 - a.m22 * a.m30)) +
                      a.m02 * (a.m10 * (a.m21 * a.m33 - a.m23 * a.m31) - a.m11 * (a.m20 * a.m33 - a.m23 * a.m30) + a.m13 * (a.m20 * a.m31 - a.m21 * a.m30)) -
                      a.m03 * (a.m10 * (a.m21 * a.m32 - a.m22 * a.m31) - a.m11 * (a.m20 * a.m32 - a.m22 * a.m30) + a.m12 * (a.m20 * a.m31 - a.m21 * a.m30));

  if (determinant == 0.0F) {
    return a;
  }

  float determinant_inv = 1.0F / determinant;

  matrix4_t m = {
    (a.m11 * (a.m22 * a.m33 - a.m23 * a.m32) - a.m12 * (a.m21 * a.m33 - a.m23 * a.m31) + a.m13 * (a.m21 * a.m32 - a.m22 * a.m31)) * determinant_inv,
    -(a.m01 * (a.m22 * a.m33 - a.m23 * a.m32) - a.m02 * (a.m21 * a.m33 - a.m23 * a.m31) + a.m03 * (a.m21 * a.m32 - a.m22 * a.m31)) * determinant_inv,
    (a.m01 * (a.m12 * a.m33 - a.m13 * a.m32) - a.m02 * (a.m11 * a.m33 - a.m13 * a.m31) + a.m03 * (a.m11 * a.m32 - a.m12 * a.m31)) * determinant_inv,
    -(a.m01 * (a.m12 * a.m23 - a.m13 * a.m22) - a.m02 * (a.m11 * a.m23 - a.m13 * a.m21) + a.m03 * (a.m11 * a.m22 - a.m12 * a.m21)) * determinant_inv,

    -(a.m10 * (a.m22 * a.m33 - a.m23 * a.m32) - a.m12 * (a.m20 * a.m33 - a.m23 * a.m30) + a.m13 * (a.m20 * a.m32 - a.m22 * a.m30)) * determinant_inv,
    (a.m00 * (a.m22 * a.m33 - a.m23 * a.m32) - a.m02 * (a.m20 * a.m33 - a.m23 * a.m30) + a.m03 * (a.m20 * a.m32 - a.m22 * a.m30)) * determinant_inv,
    -(a.m00 * (a.m12 * a.m33 - a.m13 * a.m32) - a.m02 * (a.m10 * a.m33 - a.m13 * a.m30) + a.m03 * (a.m10 * a.m32 - a.m12 * a.m30)) * determinant_inv,
    (a.m00 * (a.m12 * a.m23 - a.m13 * a.m22) - a.m02 * (a.m10 * a.m23 - a.m13 * a.m20) + a.m03 * (a.m10 * a.m22 - a.m12 * a.m20)) * determinant_inv,

    (a.m10 * (a.m21 * a.m33 - a.m23 * a.m31) - a.m11 * (a.m20 * a.m33 - a.m23 * a.m30) + a.m13 * (a.m20 * a.m31 - a.m21 * a.m30)) * determinant_inv,
    -(a.m00 * (a.m21 * a.m33 - a.m23 * a.m31) - a.m01 * (a.m20 * a.m33 - a.m23 * a.m30) + a.m03 * (a.m20 * a.m31 - a.m21 * a.m30)) * determinant_inv,
    (a.m00 * (a.m11 * a.m33 - a.m13 * a.m31) - a.m01 * (a.m10 * a.m33 - a.m13 * a.m30) + a.m03 * (a.m10 * a.m31 - a.m11 * a.m30)) * determinant_inv,
    -(a.m00 * (a.m11 * a.m23 - a.m13 * a.m21) - a.m01 * (a.m10 * a.m23 - a.m13 * a.m20) + a.m03 * (a.m10 * a.m21 - a.m11 * a.m20)) * determinant_inv,

    -(a.m10 * (a.m21 * a.m32 - a.m22 * a.m31) - a.m11 * (a.m20 * a.m32 - a.m22 * a.m30) + a.m12 * (a.m20 * a.m31 - a.m21 * a.m30)) * determinant_inv,
    (a.m00 * (a.m21 * a.m32 - a.m22 * a.m31) - a.m01 * (a.m20 * a.m32 - a.m22 * a.m30) + a.m02 * (a.m20 * a.m31 - a.m21 * a.m30)) * determinant_inv,
    -(a.m00 * (a.m11 * a.m32 - a.m12 * a.m31) - a.m01 * (a.m10 * a.m32 - a.m12 * a.m30) + a.m02 * (a.m10 * a.m31 - a.m11 * a.m30)) * determinant_inv,
    (a.m00 * (a.m11 * a.m22 - a.m12 * a.m21) - a.m01 * (a.m10 * a.m22 - a.m12 * a.m20) + a.m02 * (a.m10 * a.m21 - a.m11 * a.m20)) * determinant_inv,
  };

  return m;
}
__forceinline vector3_t matrix4_position(matrix4_t a) {
  vector3_t m = {
    a.m30,
    a.m31,
    a.m32,
  };

  return m;
}
__forceinline quaternion_t matrix4_rotation(matrix4_t a) {
  quaternion_t q = {0};

  vector3_t c0 = {a.m00, a.m10, a.m20};
  vector3_t c1 = {a.m01, a.m11, a.m21};
  vector3_t c2 = {a.m02, a.m12, a.m22};

  vector3_t cn0 = vector3_norm(c0);
  vector3_t cn1 = vector3_norm(c1);
  vector3_t cn2 = vector3_norm(c2);

  float trace = cn0.x + cn1.y + cn2.z;
  if (trace > 0.0F) {
    float s = 0.5F / sqrtf(trace + 1.0F);

    q.x = (cn2.y - cn1.z) * s;
    q.y = (cn0.z - cn2.x) * s;
    q.z = (cn1.x - cn0.y) * s;
    q.w = 0.25F / s;
  } else {
    if ((cn0.x > cn1.y) && (cn0.x > cn2.z)) {
      float s = 2.0F * sqrtf(1.0F + cn0.x - cn1.y - cn2.z);

      q.x = 0.25F * s;
      q.y = (cn0.y + cn1.x) / s;
      q.z = (cn0.z + cn2.y) / s;
      q.w = (cn2.y - cn1.z) / s;
    } else if (cn1.y > cn2.z) {
      float s = 2.0F * sqrtf(1.0F + cn1.y - cn0.x - cn2.z);

      q.x = (cn0.y + cn1.x) / s;
      q.y = 0.25F * s;
      q.z = (cn1.z + cn2.y) / s;
      q.w = (cn0.z - cn2.x) / s;
    } else {
      float s = 2.0F * sqrtf(1.0F + cn2.z - cn0.x - cn1.y);

      q.x = (cn0.z + cn2.x) / s;
      q.y = (cn1.z + cn2.y) / s;
      q.z = 0.25F * s;
      q.w = (cn1.x - cn0.y) / s;
    }
  }

  return q;
}
__forceinline vector3_t matrix4_euler_angles(matrix4_t a) {
  vector3_t v = {0};

  if (fabsf(a.m02) < (1.0F - EPSILON_6)) {
    v.x = atan2f(-a.m12, a.m22);
    v.y = asinf(a.m02);
    v.z = atan2f(-a.m01, a.m00);
  } else {
    v.x = atan2f(a.m10, a.m11);
    v.y = (a.m02 > 0.0F) ? PI_HALF : -PI_HALF;
    v.z = 0.0F;
  }

  return v;
}
__forceinline vector3_t matrix4_scale(matrix4_t a) {
  vector3_t v = {
    a.m00,
    a.m11,
    a.m22,
  };

  return v;
}
__forceinline void matrix4_decompose(matrix4_t a, vector3_t *p, quaternion_t *r, vector3_t *s) {
  p->x = a.m30;
  p->y = a.m31;
  p->z = a.m32;

  vector3_t c0 = {a.m00, a.m10, a.m20};
  vector3_t c1 = {a.m01, a.m11, a.m21};
  vector3_t c2 = {a.m02, a.m12, a.m22};

  s->x = vector3_length(c0);
  s->y = vector3_length(c1);
  s->z = vector3_length(c2);

  vector3_t cn0 = vector3_norm(c0);
  vector3_t cn1 = vector3_norm(c1);
  vector3_t cn2 = vector3_norm(c2);

  float trace = cn0.x + cn1.y + cn2.z;
  if (trace > 0.0F) {
    float s = 0.5F / sqrtf(trace + 1.0F);

    r->x = (cn2.y - cn1.z) * s;
    r->y = (cn0.z - cn2.x) * s;
    r->z = (cn1.x - cn0.y) * s;
    r->w = 0.25F / s;
  } else {
    if ((cn0.x > cn1.y) && (cn0.x > cn2.z)) {
      float s = 2.0F * sqrtf(1.0F + cn0.x - cn1.y - cn2.z);

      r->x = 0.25F * s;
      r->y = (cn0.y + cn1.x) / s;
      r->z = (cn0.z + cn2.y) / s;
      r->w = (cn2.y - cn1.z) / s;
    } else if (cn1.y > cn2.z) {
      float s = 2.0F * sqrtf(1.0F + cn1.y - cn0.x - cn2.z);

      r->x = (cn0.y + cn1.x) / s;
      r->y = 0.25F * s;
      r->z = (cn1.z + cn2.y) / s;
      r->w = (cn0.z - cn2.x) / s;
    } else {
      float s = 2.0F * sqrtf(1.0F + cn2.z - cn0.x - cn1.y);

      r->x = (cn0.z + cn2.x) / s;
      r->y = (cn1.z + cn2.y) / s;
      r->z = 0.25F * s;
      r->w = (cn1.x - cn0.y) / s;
    }
  }
}
__forceinline matrix4_t matrix4_mul(matrix4_t a, matrix4_t b) {
  matrix4_t m = {
    (a.m00 * b.m00) + (a.m01 * b.m10) + (a.m02 * b.m20) + (a.m03 * b.m30),
    (a.m00 * b.m01) + (a.m01 * b.m11) + (a.m02 * b.m21) + (a.m03 * b.m31),
    (a.m00 * b.m02) + (a.m01 * b.m12) + (a.m02 * b.m22) + (a.m03 * b.m32),
    (a.m00 * b.m03) + (a.m01 * b.m13) + (a.m02 * b.m23) + (a.m03 * b.m33),

    (a.m10 * b.m00) + (a.m11 * b.m10) + (a.m12 * b.m20) + (a.m13 * b.m30),
    (a.m10 * b.m01) + (a.m11 * b.m11) + (a.m12 * b.m21) + (a.m13 * b.m31),
    (a.m10 * b.m02) + (a.m11 * b.m12) + (a.m12 * b.m22) + (a.m13 * b.m32),
    (a.m10 * b.m03) + (a.m11 * b.m13) + (a.m12 * b.m23) + (a.m13 * b.m33),

    (a.m20 * b.m00) + (a.m21 * b.m10) + (a.m22 * b.m20) + (a.m23 * b.m30),
    (a.m20 * b.m01) + (a.m21 * b.m11) + (a.m22 * b.m21) + (a.m23 * b.m31),
    (a.m20 * b.m02) + (a.m21 * b.m12) + (a.m22 * b.m22) + (a.m23 * b.m32),
    (a.m20 * b.m03) + (a.m21 * b.m13) + (a.m22 * b.m23) + (a.m23 * b.m33),

    (a.m30 * b.m00) + (a.m31 * b.m10) + (a.m32 * b.m20) + (a.m33 * b.m30),
    (a.m30 * b.m01) + (a.m31 * b.m11) + (a.m32 * b.m21) + (a.m33 * b.m31),
    (a.m30 * b.m02) + (a.m31 * b.m12) + (a.m32 * b.m22) + (a.m33 * b.m32),
    (a.m30 * b.m03) + (a.m31 * b.m13) + (a.m32 * b.m23) + (a.m33 * b.m33),
  };

  return m;
}
__forceinline vector4_t matrix4_mul_vector(matrix4_t a, vector4_t b) {
  vector4_t v = {
    (a.m00 * b.x) + (a.m01 * b.y) + (a.m02 * b.z) + (a.m03 * b.w),
    (a.m10 * b.x) + (a.m11 * b.y) + (a.m12 * b.z) + (a.m13 * b.w),
    (a.m20 * b.x) + (a.m21 * b.y) + (a.m22 * b.z) + (a.m23 * b.w),
    (a.m30 * b.x) + (a.m31 * b.y) + (a.m32 * b.z) + (a.m33 * b.w),
  };

  return v;
}
__forceinline matrix4_t matrix4_ortho(float left, float right, float bottom, float top, float near_z, float far_z) {
  matrix4_t m = {
    2.0F / (right - left),
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    2.0F / (top - bottom),
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    -2.0F / (near_z - far_z),
    0.0F,
    -(right + left) / (right - left),
    -(top + bottom) / (top - bottom),
    -(near_z + far_z) / (near_z - far_z),
    1.0F,
  };

  return m;
}
__forceinline matrix4_t matrix4_persp(float fov, float aspect_ratio, float near_z, float far_z) {
  float tan_half_fov = tanf(fov * 0.5F);

  matrix4_t m = {
    1.0F / (aspect_ratio * tan_half_fov),
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    1.0F / tan_half_fov,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    far_z / (far_z - near_z),
    1.0F,
    0.0F,
    0.0F,
    -(far_z * near_z) / (far_z - near_z),
    0.0F,
  };

  return m;
}
__forceinline matrix4_t matrix4_look_at(vector3_t eye, vector3_t center, vector3_t up) {
  vector3_t f = vector3_norm(vector3_sub(center, eye));
  vector3_t r = vector3_norm(vector3_cross(f, up));
  vector3_t u = vector3_cross(r, f);

  matrix4_t m = {
    r.x,
    u.x,
    f.x,
    0.0F,
    r.y,
    u.y,
    f.y,
    0.0F,
    r.z,
    u.z,
    f.z,
    0.0F,
    -vector3_dot(r, eye),
    -vector3_dot(u, eye),
    -vector3_dot(f, eye),
    1.0F,
  };

  return m;
}
__forceinline void matrix4_print(matrix4_t a) {
  printf("[%f, %f, %f, %f]\n", a.m00, a.m01, a.m02, a.m03);
  printf("[%f, %f, %f, %f]\n", a.m10, a.m11, a.m12, a.m13);
  printf("[%f, %f, %f, %f]\n", a.m20, a.m21, a.m22, a.m23);
  printf("[%f, %f, %f, %f]\n", a.m30, a.m31, a.m32, a.m33);
}
