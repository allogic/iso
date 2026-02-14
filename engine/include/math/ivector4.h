#ifndef MATH_IVECTOR4_H
#define MATH_IVECTOR4_H

#include <fwd.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline ivector4_t ivector4_zero(void);
__forceinline ivector4_t ivector4_one(void);
__forceinline ivector4_t ivector4_xyzw(int32_t x, int32_t y, int32_t z, int32_t w);
__forceinline ivector4_t ivector4_negate(ivector4_t a);
__forceinline ivector4_t ivector4_add(ivector4_t a, ivector4_t b);
__forceinline ivector4_t ivector4_sub(ivector4_t a, ivector4_t b);
__forceinline ivector4_t ivector4_mul(ivector4_t a, ivector4_t b);
__forceinline ivector4_t ivector4_div(ivector4_t a, ivector4_t b);
__forceinline ivector4_t ivector4_adds(ivector4_t a, int32_t b);
__forceinline ivector4_t ivector4_subs(ivector4_t a, int32_t b);
__forceinline ivector4_t ivector4_muls(ivector4_t a, int32_t b);
__forceinline ivector4_t ivector4_divs(ivector4_t a, int32_t b);
__forceinline float ivector4_dot(ivector4_t a, ivector4_t b);
__forceinline float ivector4_length(ivector4_t a);
__forceinline float ivector4_length2(ivector4_t a);
__forceinline void ivector4_print(ivector4_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/ivector4.inl>

#endif // MATH_IVECTOR4_H
