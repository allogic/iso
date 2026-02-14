#ifndef NOISE_FBM_H
#define NOISE_FBM_H

#include "simplex.glsl"
#include "gradient.glsl"

float noise_fbm(vec2 position, float scale, float amplitude, uint octaves) {
	float v = 0.0;
	float a = 0.5;

	for (int i = 0; i < octaves; i++)
	{
		v += a * noise_simplex(position);

		position *= scale;

		a *= amplitude;
	}

	return v;
}

float noise_fbm(vec3 position, float scale, float amplitude, uint octaves) {
	float v = 0.0;
	float a = 0.5;

	for (int i = 0; i < octaves; i++)
	{
		v += a * noise_simplex(position);

		position *= scale;

		a *= amplitude;
	}

	return v;
}

float noise_fbm(vec3 position, float tile_length, float lacunarity, float persistence, int octaves) {
	float a = 0.5;
	float t = 0.0;
	float n = 0.0;

	for (int i = 0; i < octaves; i++)
	{
		float v = noise_gradient(position, tile_length * lacunarity * 0.5) * 0.5 + 0.5;

		t += v * a;
		n += a;
		a *= persistence;

		position *= lacunarity;
	}

	return t / n;
}

#endif // NOISE_FBM_H
