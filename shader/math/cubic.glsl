#ifndef MATH_CUBIC_H
#define MATH_CUBIC_H

float cubic(float v) {
	return v * v * (3.0 - 2.0 * v);
}

vec2 cubic(vec2 v) {
	return v * v * (3.0 - 2.0 * v);
}

vec3 cubic(vec3 v) {
	return v * v * (3.0 - 2.0 * v);
}

vec4 cubic(vec4 v) {
	return v * v * (3.0 - 2.0 * v);
}

float cubic(float v, float slope0, float slope1) {
	float a = slope0 + slope1 - 2.0;
	float b = -2.0 * slope0 - slope1 + 3.0;
	float c = slope0;

	float v2 = v * v;
	float v3 = v * v2;

	return a * v3 + b * v2 + c * v;
}

vec2 cubic(vec2 v, float slope0, float slope1) {
	float a = slope0 + slope1 - 2.0;
	float b = -2.0 * slope0 - slope1 + 3.0;
	float c = slope0;

	vec2 v2 = v * v;
	vec2 v3 = v * v2;

	return a * v3 + b * v2 + c * v;
}

vec3 cubic(vec3 v, float slope0, float slope1) {
	float a = slope0 + slope1 - 2.0;
	float b = -2.0 * slope0 - slope1 + 3.0;
	float c = slope0;

	vec3 v2 = v * v;
	vec3 v3 = v * v2;

	return a * v3 + b * v2 + c * v;
}

vec4 cubic(vec4 v, float slope0, float slope1) {
	float a = slope0 + slope1 - 2.0;
	float b = -2.0 * slope0 - slope1 + 3.0;
	float c = slope0;

	vec4 v2 = v * v;
	vec4 v3 = v * v2;

	return a * v3 + b * v2 + c * v;
}

#endif // MATH_CUBIC_H
