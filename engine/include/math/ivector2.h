#ifndef MATH_IVECTOR2_H
#define MATH_IVECTOR2_H

#include <fwd.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline ivector2_t ivector2_zero(void);
__forceinline ivector2_t ivector2_one(void);
__forceinline ivector2_t ivector2_right(void);
__forceinline ivector2_t ivector2_up(void);
__forceinline ivector2_t ivector2_left(void);
__forceinline ivector2_t ivector2_down(void);
__forceinline ivector2_t ivector2_xy(int32_t x, int32_t y);
__forceinline ivector2_t ivector2_negate(ivector2_t a);
__forceinline ivector2_t ivector2_add(ivector2_t a, ivector2_t b);
__forceinline ivector2_t ivector2_sub(ivector2_t a, ivector2_t b);
__forceinline ivector2_t ivector2_mul(ivector2_t a, ivector2_t b);
__forceinline ivector2_t ivector2_div(ivector2_t a, ivector2_t b);
__forceinline ivector2_t ivector2_adds(ivector2_t a, int32_t b);
__forceinline ivector2_t ivector2_subs(ivector2_t a, int32_t b);
__forceinline ivector2_t ivector2_muls(ivector2_t a, int32_t b);
__forceinline ivector2_t ivector2_divs(ivector2_t a, int32_t b);
__forceinline float ivector2_dot(ivector2_t a, ivector2_t b);
__forceinline float ivector2_length(ivector2_t a);
__forceinline float ivector2_length2(ivector2_t a);
__forceinline void ivector2_print(ivector2_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/ivector2.inl>

#endif // MATH_IVECTOR2_H
