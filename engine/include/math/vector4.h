#ifndef MATH_VECTOR4_H
#define MATH_VECTOR4_H

#include <fwd.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline vector4_t vector4_zero(void);
__forceinline vector4_t vector4_one(void);
__forceinline vector4_t vector4_xyzw(float x, float y, float z, float w);
__forceinline vector4_t vector4_negate(vector4_t a);
__forceinline vector4_t vector4_add(vector4_t a, vector4_t b);
__forceinline vector4_t vector4_sub(vector4_t a, vector4_t b);
__forceinline vector4_t vector4_mul(vector4_t a, vector4_t b);
__forceinline vector4_t vector4_div(vector4_t a, vector4_t b);
__forceinline vector4_t vector4_adds(vector4_t a, float b);
__forceinline vector4_t vector4_subs(vector4_t a, float b);
__forceinline vector4_t vector4_muls(vector4_t a, float b);
__forceinline vector4_t vector4_divs(vector4_t a, float b);
__forceinline vector4_t vector4_norm(vector4_t a);
__forceinline vector4_t vector4_inv(vector4_t a);
__forceinline vector4_t vector4_floor(vector4_t a);
__forceinline vector4_t vector4_ceil(vector4_t a);
__forceinline float vector4_dot(vector4_t a, vector4_t b);
__forceinline float vector4_length(vector4_t a);
__forceinline float vector4_length2(vector4_t a);
__forceinline void vector4_print(vector4_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/vector4.inl>

#endif // MATH_VECTOR4_H
