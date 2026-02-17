__forceinline quaternion_t quaternion_zero(void) {
  quaternion_t q = {
    0.0F,
    0.0F,
    0.0F,
    0.0F,
  };

  return q;
}
__forceinline quaternion_t quaternion_identity(void) {
  quaternion_t q = {
    0.0F,
    0.0F,
    0.0F,
    1.0F,
  };

  return q;
}
__forceinline quaternion_t quaternion_xyzw(float x, float y, float z, float w) {
  quaternion_t q = {
    x,
    y,
    z,
    w,
  };

  return q;
}
__forceinline quaternion_t quaternion_add(quaternion_t a, quaternion_t b) {
  quaternion_t q = {
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
    a.w + b.w,
  };

  return q;
}
__forceinline quaternion_t quaternion_adds(quaternion_t a, float b) {
  quaternion_t q = {
    a.x + b,
    a.y + b,
    a.z + b,
    a.w + b,
  };

  return q;
}
__forceinline quaternion_t quaternion_mul(quaternion_t a, quaternion_t b) {
  quaternion_t q = {
    (a.w * b.x) + (a.x * b.w) + (a.y * b.z) - (a.z * b.y),
    (a.w * b.y) - (a.x * b.z) + (a.y * b.w) + (a.z * b.x),
    (a.w * b.z) + (a.x * b.y) - (a.y * b.x) + (a.z * b.w),
    (a.w * b.w) - (a.x * b.x) - (a.y * b.y) - (a.z * b.z),
  };

  return q;
}
__forceinline quaternion_t quaternion_muls(quaternion_t a, float b) {
  quaternion_t q = {
    a.x * b,
    a.y * b,
    a.z * b,
    a.w * b,
  };

  return q;
}
__forceinline quaternion_t quaternion_conjugate(quaternion_t a) {
  quaternion_t q = {
    -a.x,
    -a.y,
    -a.z,
    a.w,
  };

  return q;
}
__forceinline vector3_t quaternion_right(quaternion_t a) {
  vector3_t q = {
    1.0F - 2.0F * ((a.y * a.y) + (a.z * a.z)),
    2.0F * ((a.x * a.y) + (a.w * a.z)),
    2.0F * ((a.x * a.z) - (a.w * a.y)),
  };

  return q;
}
__forceinline vector3_t quaternion_up(quaternion_t a) {
  vector3_t q = {
    2.0F * ((a.x * a.y) - (a.w * a.z)),
    1.0F - 2.0F * ((a.x * a.x) + (a.z * a.z)),
    2.0F * ((a.y * a.z) + (a.w * a.x)),
  };

  return q;
}
__forceinline vector3_t quaternion_front(quaternion_t a) {
  vector3_t q = {
    2.0F * ((a.x * a.z) + (a.w * a.y)),
    2.0F * ((a.y * a.z) - (a.w * a.x)),
    1.0F - 2.0F * ((a.x * a.x) + (a.y * a.y)),
  };

  return q;
}
__forceinline vector3_t quaternion_left(quaternion_t a) {
  vector3_t q = {
    -(1.0F - 2.0F * ((a.y * a.y) + (a.z * a.z))),
    -(2.0F * ((a.x * a.y) + (a.w * a.z))),
    -(2.0F * ((a.x * a.z) - (a.w * a.y))),
  };

  return q;
}
__forceinline vector3_t quaternion_down(quaternion_t a) {
  vector3_t q = {
    -(2.0F * ((a.x * a.y) - (a.w * a.z))),
    -(1.0F - 2.0F * ((a.x * a.x) + (a.z * a.z))),
    -(2.0F * ((a.y * a.z) + (a.w * a.x))),
  };

  return q;
}
__forceinline vector3_t quaternion_back(quaternion_t a) {
  vector3_t q = {
    -(2.0F * ((a.x * a.z) + (a.w * a.y))),
    -(2.0F * ((a.y * a.z) - (a.w * a.x))),
    -(1.0F - 2.0F * ((a.x * a.x) + (a.y * a.y))),
  };

  return q;
}
__forceinline vector3_t quaternion_to_euler_angles(quaternion_t a) {
  float pitch = 0.0F;
  float yaw = 0.0F;
  float roll = 0.0F;

  float test = (a.w * a.x) - (a.y * a.z);

  if (test > (0.5F - EPSILON_6)) {
    pitch = PI_HALF;
    yaw = 2.0F * atan2f(a.z, a.w);
    roll = 0.0F;
  } else if (test < -(0.5F - EPSILON_6)) {
    pitch = -PI_HALF;
    yaw = -2.0F * atan2f(a.z, a.w);
    roll = 0.0F;
  } else {
    pitch = asinf(2.0F * test);
    yaw = atan2f(2.0F * ((a.w * a.y) - (a.x * a.z)), 1.0F - 2.0F * ((a.x * a.x) + (a.y * a.y)));
    roll = atan2f(2.0F * ((a.w * a.z) - (a.x * a.y)), 1.0F - 2.0F * ((a.y * a.y) + (a.z * a.z)));
  }

  vector3_t q = {
    pitch,
    yaw,
    roll,
  };

  return q;
}
__forceinline vector3_t quaternion_to_euler_angles_xyzw(float x, float y, float z, float w) {
  float pitch = 0.0F;
  float yaw = 0.0F;
  float roll = 0.0F;

  float test = (w * x) - (y * z);

  if (test > (0.5F - EPSILON_6)) {
    pitch = PI_HALF;
    yaw = 2.0F * atan2f(z, w);
    roll = 0.0F;
  } else if (test < -(0.5F - EPSILON_6)) {
    pitch = -PI_HALF;
    yaw = -2.0F * atan2f(z, w);
    roll = 0.0F;
  } else {
    pitch = asinf(2.0F * test);
    yaw = atan2f(2.0F * ((w * y) - (x * z)), 1.0F - 2.0F * ((x * x) + (y * y)));
    roll = atan2f(2.0F * ((w * z) - (x * y)), 1.0F - 2.0F * ((y * y) + (z * z)));
  }

  vector3_t q = {
    pitch,
    yaw,
    roll,
  };

  return q;
}
__forceinline quaternion_t quaternion_from_euler_angles(vector3_t a) {
  float sp = sinf(a.x * 0.5F);
  float sy = sinf(a.y * 0.5F);
  float sr = sinf(a.z * 0.5F);

  float cp = cosf(a.x * 0.5F);
  float cy = cosf(a.y * 0.5F);
  float cr = cosf(a.z * 0.5F);

  quaternion_t q = {
    sr * cp * cy - cr * sp * sy,
    cr * sp * cy + sr * cp * sy,
    cr * cp * sy - sr * sp * cy,
    cr * cp * cy + sr * sp * sy,
  };

  return q;
}
__forceinline quaternion_t quaternion_from_euler_angles_pyr(float p, float y, float r) {
  float sp = sinf(p * 0.5F);
  float sy = sinf(y * 0.5F);
  float sr = sinf(r * 0.5F);

  float cp = cosf(p * 0.5F);
  float cy = cosf(y * 0.5F);
  float cr = cosf(r * 0.5F);

  quaternion_t q = {
    sr * cp * cy - cr * sp * sy,
    cr * sp * cy + sr * cp * sy,
    cr * cp * sy - sr * sp * cy,
    cr * cp * cy + sr * sp * sy,
  };

  return q;
}
__forceinline quaternion_t quaternion_angle_axis(float a, vector3_t b) {
  vector3_t n = vector3_norm(b);

  float a_half = a * 0.5F;
  float s = sinf(a_half);

  quaternion_t q = {
    n.x * s,
    n.y * s,
    n.z * s,
    cosf(a_half),
  };

  return q;
}
__forceinline quaternion_t quaternion_norm(quaternion_t a) {
  float l = quaternion_length(a);

  if (l > 0.0F) {
    return quaternion_muls(a, 1.0F / l);
  } else {
    return quaternion_identity();
  }
}
__forceinline float quaternion_dot(quaternion_t a, quaternion_t b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}
__forceinline float quaternion_length(quaternion_t a) {
  return sqrtf(quaternion_dot(a, a));
}
__forceinline float quaternion_length2(quaternion_t a) {
  return quaternion_dot(a, a);
}
__forceinline void quaternion_print(quaternion_t a) {
  printf("[%f, %f, %f, %f]\n", a.x, a.y, a.z, a.w);
}
