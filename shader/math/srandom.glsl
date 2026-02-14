#ifndef MATH_SRANDOM_H
#define MATH_SRANDOM_H

float srandom(float x) {
	return -1.0 + 2.0 * fract(sin(x) * 43758.5453);
}

float srandom(vec2 st) {
	return -1.0 + 2.0 * fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

float srandom(vec3 pos) {
	return -1.0 + 2.0 * fract(sin(dot(pos.xyz, vec3(70.9898, 78.233, 32.4355))) * 43758.5453123);
}

float srandom(vec4 pos) {
	float dot_product = dot(pos, vec4(12.9898, 78.233, 45.164, 94.673));

	return -1.0 + 2.0 * fract(sin(dot_product) * 43758.5453);
}

vec2 srandom2(vec2 st) {
	const vec2 k = vec2(0.3183099, 0.3678794);

	st = st * k + k.yx;

	return -1.0 + 2.0 * fract(16.0 * k * fract(st.x * st.y * (st.x + st.y)));
}

vec3 srandom3(vec3 p) {
	p = vec3(dot(p, vec3(127.1, 311.7, 74.7)), dot(p, vec3(269.5, 183.3, 246.1)), dot(p, vec3(113.5, 271.9, 124.6)));

	return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}

vec2 srandom2(vec2 p, float tile_length) {
	p = mod(p, vec2(tile_length));

	return srandom2(p);
}

vec3 srandom3(vec3 p, float tile_length) {
	p = mod(p, vec3(tile_length));

	return srandom3(p);
}

#endif // MATH_SRANDOM_H
