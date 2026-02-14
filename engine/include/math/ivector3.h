#ifndef MATH_IVECTOR3_H
#define MATH_IVECTOR3_H

#include <fwd.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline ivector3_t ivector3_zero(void);
__forceinline ivector3_t ivector3_one(void);
__forceinline ivector3_t ivector3_right(void);
__forceinline ivector3_t ivector3_up(void);
__forceinline ivector3_t ivector3_front(void);
__forceinline ivector3_t ivector3_left(void);
__forceinline ivector3_t ivector3_down(void);
__forceinline ivector3_t ivector3_back(void);
__forceinline ivector3_t ivector3_xyz(int32_t x, int32_t y, int32_t z);
__forceinline ivector3_t ivector3_negate(ivector3_t a);
__forceinline ivector3_t ivector3_add(ivector3_t a, ivector3_t b);
__forceinline ivector3_t ivector3_sub(ivector3_t a, ivector3_t b);
__forceinline ivector3_t ivector3_mul(ivector3_t a, ivector3_t b);
__forceinline ivector3_t ivector3_div(ivector3_t a, ivector3_t b);
__forceinline ivector3_t ivector3_adds(ivector3_t a, int32_t b);
__forceinline ivector3_t ivector3_subs(ivector3_t a, int32_t b);
__forceinline ivector3_t ivector3_muls(ivector3_t a, int32_t b);
__forceinline ivector3_t ivector3_divs(ivector3_t a, int32_t b);
__forceinline float ivector3_dot(ivector3_t a, ivector3_t b);
__forceinline float ivector3_length(ivector3_t a);
__forceinline float ivector3_length2(ivector3_t a);
__forceinline void ivector3_print(ivector3_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/ivector3.inl>

#endif // MATH_IVECTOR3_H
