#ifndef NOISE_CURL_H
#define NOISE_CURL_H

#include "simplex.glsl"

vec2 noise_curl_t0(vec2 position) {
	const float e = 0.1;

	vec2 dx = vec2(e, 0.0);
	vec2 dy = vec2(0.0, e);

	vec2 p_x0 = noise_simplex2(position - dx);
	vec2 p_x1 = noise_simplex2(position + dx);
	vec2 p_y0 = noise_simplex2(position - dy);
	vec2 p_y1 = noise_simplex2(position + dy);

	float x = p_x1.y + p_x0.y;
	float y = p_y1.x - p_y0.x;

	const float divisor = 1.0 / (2.0 * e);

	return normalize(vec2(x, y) * divisor);
}

vec3 noise_curl_t1(vec3 position) {
	const float e = 0.1;

	vec3 dx = vec3(e, 0.0, 0.0);
	vec3 dy = vec3(0.0, e, 0.0);
	vec3 dz = vec3(0.0, 0.0, e);

	vec3 p_x0 = noise_simplex3(position - dx);
	vec3 p_x1 = noise_simplex3(position + dx);
	vec3 p_y0 = noise_simplex3(position - dy);
	vec3 p_y1 = noise_simplex3(position + dy);
	vec3 p_z0 = noise_simplex3(position - dz);
	vec3 p_z1 = noise_simplex3(position + dz);

	float x = p_y1.z - p_y0.z - p_z1.y + p_z0.y;
	float y = p_z1.x - p_z0.x - p_x1.z + p_x0.z;
	float z = p_x1.y - p_x0.y - p_y1.x + p_y0.x;

	const float divisor = 1.0 / (2.0 * e);

	return normalize(vec3(x, y, z) * divisor);
}

vec3 noise_curl_t2(vec4 position) {
	const float e = 0.1;

	vec4 dx = vec4(e, 0.0, 0.0, 1.0);
	vec4 dy = vec4(0.0, e, 0.0, 1.0);
	vec4 dz = vec4(0.0, 0.0, e, 1.0);

	vec3 p_x0 = noise_simplex4(position - dx);
	vec3 p_x1 = noise_simplex4(position + dx);
	vec3 p_y0 = noise_simplex4(position - dy);
	vec3 p_y1 = noise_simplex4(position + dy);
	vec3 p_z0 = noise_simplex4(position - dz);
	vec3 p_z1 = noise_simplex4(position + dz);

	float x = p_y1.z - p_y0.z - p_z1.y + p_z0.y;
	float y = p_z1.x - p_z0.x - p_x1.z + p_x0.z;
	float z = p_x1.y - p_x0.y - p_y1.x + p_y0.x;

	const float divisor = 1.0 / (2.0 * e);

	return normalize(vec3(x, y, z) * divisor);
}

#endif // NOISE_CURL_H
