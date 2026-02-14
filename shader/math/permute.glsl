#ifndef MATH_PERMUTE_H
#define MATH_PERMUTE_H

#include "mod289.glsl"

float permute(float v) {
	return mod289(((v * 34.0) + 1.0) * v);
}

vec2 permute(vec2 v) {
	return mod289(((v * 34.0) + 1.0) * v);
}

vec3 permute(vec3 v) {
	return mod289(((v * 34.0) + 1.0) * v);
}

vec4 permute(vec4 v) {
	return mod289(((v * 34.0) + 1.0) * v);
}

#endif // MATH_PERMUTE_H
