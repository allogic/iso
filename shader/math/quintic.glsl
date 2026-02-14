#ifndef MATH_QUINTIC_H
#define MATH_QUINTIC_H

float quintic(float v) {
	return v * v * v * (v * (v * 6.0 - 15.0) + 10.0);
}

vec2 quintic(vec2 v) {
	return v * v * v * (v * (v * 6.0 - 15.0) + 10.0);
}

vec3 quintic(vec3 v) {
	return v * v * v * (v * (v * 6.0 - 15.0) + 10.0);
}

vec4 quintic(vec4 v) {
	return v * v * v * (v * (v * 6.0 - 15.0) + 10.0);
}

#endif // MATH_QUINTIC_H
