#ifndef MATH_MISC_H
#define MATH_MISC_H

#include <fwd.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline float deg_to_rad(float a);
__forceinline float rad_to_deg(float a);

__forceinline float clampf(float a, float min, float max);
__forceinline int32_t clampi(int32_t a, int32_t min, int32_t max);

__forceinline float fsignf(float a);

__forceinline int32_t vec_to_index(ivector3_t position, ivector3_t size);
__forceinline ivector3_t index_to_vec(int32_t index, ivector3_t size);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/misc.inl>

#endif // MATH_MISC_H
