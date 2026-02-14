#ifndef MATH_QUATERNION_H
#define MATH_QUATERNION_H

#include <fwd.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline quaternion_t quaternion_zero(void);
__forceinline quaternion_t quaternion_identity(void);
__forceinline quaternion_t quaternion_xyzw(float x, float y, float z, float w);
__forceinline quaternion_t quaternion_add(quaternion_t a, quaternion_t b);
__forceinline quaternion_t quaternion_adds(quaternion_t a, float b);
__forceinline quaternion_t quaternion_mul(quaternion_t a, quaternion_t b);
__forceinline quaternion_t quaternion_muls(quaternion_t a, float b);
__forceinline quaternion_t quaternion_conjugate(quaternion_t a);
__forceinline vector3_t quaternion_right(quaternion_t a);
__forceinline vector3_t quaternion_up(quaternion_t a);
__forceinline vector3_t quaternion_front(quaternion_t a);
__forceinline vector3_t quaternion_left(quaternion_t a);
__forceinline vector3_t quaternion_down(quaternion_t a);
__forceinline vector3_t quaternion_back(quaternion_t a);
__forceinline vector3_t quaternion_to_euler_angles(quaternion_t a);
__forceinline vector3_t quaternion_to_euler_angles_xyzw(float x, float y, float z, float w);
__forceinline quaternion_t quaternion_from_euler_angles(vector3_t a);
__forceinline quaternion_t quaternion_from_euler_angles_pyr(float p, float y, float r);
__forceinline quaternion_t quaternion_angle_axis(float a, vector3_t b);
__forceinline quaternion_t quaternion_norm(quaternion_t a);
__forceinline float quaternion_dot(quaternion_t a, quaternion_t b);
__forceinline float quaternion_length(quaternion_t a);
__forceinline float quaternion_length2(quaternion_t a);
__forceinline void quaternion_print(quaternion_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/quaternion.inl>

#endif // MATH_QUATERNION_H
