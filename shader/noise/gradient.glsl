#ifndef NOISE_GRADIENT_H
#define NOISE_GRADIENT_H

#include "../math/urandom.glsl"
#include "../math/cubic.glsl"
#include "../math/quintic.glsl"
#include "../math/srandom.glsl"

float noise_gradient(float position) {
	float i = floor(position);
	float f = fract(position);

	return mix(urandom(i), urandom(i + 1.0), smoothstep(0.0, 1.0, f));
}

float noise_gradient(vec2 position) {
	vec2 i = floor(position);
	vec2 f = fract(position);

	float a = urandom(i);
	float b = urandom(i + vec2(1.0, 0.0));
	float c = urandom(i + vec2(0.0, 1.0));
	float d = urandom(i + vec2(1.0, 1.0));

	vec2 u = cubic(f);

	return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

float noise_gradient(vec3 position) {
	vec3 i = floor(position);
	vec3 f = fract(position);

	vec3 u = quintic(f);

	return -1.0 + 2.0 * mix(mix(mix(urandom(i + vec3(0.0, 0.0, 0.0)), urandom(i + vec3(1.0, 0.0, 0.0)), u.x), mix(urandom(i + vec3(0.0, 1.0, 0.0)), urandom(i + vec3(1.0, 1.0, 0.0)), u.x), u.y), mix(mix(urandom(i + vec3(0.0, 0.0, 1.0)), urandom(i + vec3(1.0, 0.0, 1.0)), u.x), mix(urandom(i + vec3(0.0, 1.0, 1.0)), urandom(i + vec3(1.0, 1.0, 1.0)), u.x), u.y), u.z);
}

float noise_gradient(vec3 position, float tile_length) {
	vec3 i = floor(position);
	vec3 f = fract(position);

	vec3 u = quintic(f);

	return mix(mix(mix(dot(srandom3(i + vec3(0.0, 0.0, 0.0), tile_length), f - vec3(0.0, 0.0, 0.0)), dot(srandom3(i + vec3(1.0, 0.0, 0.0), tile_length), f - vec3(1.0, 0.0, 0.0)), u.x), mix(dot(srandom3(i + vec3(0.0, 1.0, 0.0), tile_length), f - vec3(0.0, 1.0, 0.0)), dot(srandom3(i + vec3(1.0, 1.0, 0.0), tile_length), f - vec3(1.0, 1.0, 0.0)), u.x), u.y), mix(mix(dot(srandom3(i + vec3(0.0, 0.0, 1.0), tile_length), f - vec3(0.0, 0.0, 1.0)), dot(srandom3(i + vec3(1.0, 0.0, 1.0), tile_length), f - vec3(1.0, 0.0, 1.0)), u.x), mix(dot(srandom3(i + vec3(0.0, 1.0, 1.0), tile_length), f - vec3(0.0, 1.0, 1.0)), dot(srandom3(i + vec3(1.0, 1.0, 1.0), tile_length), f - vec3(1.0, 1.0, 1.0)), u.x), u.y), u.z);
}

#endif // NOISE_GRADIENT_H
