#ifndef MATH_VECTOR2_H
#define MATH_VECTOR2_H

#include <fwd.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline vector2_t vector2_zero(void);
__forceinline vector2_t vector2_one(void);
__forceinline vector2_t vector2_right(void);
__forceinline vector2_t vector2_up(void);
__forceinline vector2_t vector2_left(void);
__forceinline vector2_t vector2_down(void);
__forceinline vector2_t vector2_xy(float x, float y);
__forceinline vector2_t vector2_negate(vector2_t a);
__forceinline vector2_t vector2_add(vector2_t a, vector2_t b);
__forceinline vector2_t vector2_sub(vector2_t a, vector2_t b);
__forceinline vector2_t vector2_mul(vector2_t a, vector2_t b);
__forceinline vector2_t vector2_div(vector2_t a, vector2_t b);
__forceinline vector2_t vector2_adds(vector2_t a, float b);
__forceinline vector2_t vector2_subs(vector2_t a, float b);
__forceinline vector2_t vector2_muls(vector2_t a, float b);
__forceinline vector2_t vector2_divs(vector2_t a, float b);
__forceinline vector2_t vector2_norm(vector2_t a);
__forceinline vector2_t vector2_inv(vector2_t a);
__forceinline vector2_t vector2_floor(vector2_t a);
__forceinline vector2_t vector2_ceil(vector2_t a);
__forceinline float vector2_dot(vector2_t a, vector2_t b);
__forceinline float vector2_length(vector2_t a);
__forceinline float vector2_length2(vector2_t a);
__forceinline void vector2_print(vector2_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/vector2.inl>

#endif // MATH_VECTOR2_H
