#ifndef MATH_TAYLER_INV_SQRT_H
#define MATH_TAYLER_INV_SQRT_H

float taylor_inv_sqrt(float r) {
	return 1.79284291400159 - 0.85373472095314 * r;
}

vec2 taylor_inv_sqrt(vec2 r) {
	return 1.79284291400159 - 0.85373472095314 * r;
}

vec3 taylor_inv_sqrt(vec3 r) {
	return 1.79284291400159 - 0.85373472095314 * r;
}

vec4 taylor_inv_sqrt(vec4 r) {
	return 1.79284291400159 - 0.85373472095314 * r;
}

#endif // MATH_TAYLER_INV_SQRT_H
