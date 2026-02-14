#ifndef NOISE_CELLULAR_H
#define NOISE_CELLULAR_H

#include "../math/urandom.glsl"

float noise_cellular_t0(vec2 position, float u, float v) {
	float k = 1.0 + 63.0 * pow(1.0 - v, 6.0);

	vec2 i = floor(position);
	vec2 f = fract(position);

	vec2 a = vec2(0.0, 0.0);
	vec2 g = vec2(-2.0);

	for (g.y = -2.0; g.y <= 2.0; g.y++)
	{
		for (g.x = -2.0; g.x <= 2.0; g.x++)
		{
			vec3 o = urandom3(i + g) * vec3(u, u, 1.0);
			vec2 d = g - f + o.xy;

			float w = pow(1.0 - smoothstep(0.0, 1.414, length(d)), k);

			a += vec2(o.z * w, w);
		}
	}

	return a.x / a.y;
}

float noise_cellular_t1(vec3 position, float u, float v) {
	float k = 1.0 + 63.0 * pow(1.0 - v, 6.0);

	vec3 i = floor(position);
	vec3 f = fract(position);

	float s = 1.0 + 31.0 * v;

	vec2 a = vec2(0.0, 0.0);
	vec3 g = vec3(-2.0);

	for (g.z = -2.0; g.z <= 2.0; g.z++)
	{
		for (g.y = -2.0; g.y <= 2.0; g.y++)
		{
			for (g.x = -2.0; g.x <= 2.0; g.x++)
			{
				vec3 o = urandom3(i + g) * vec3(u, u, 1.0);
				vec3 d = g - f + o + 0.5;

				float w = pow(1.0 - smoothstep(0.0, 1.414, length(d)), k);

				a += vec2(o.z * w, w);
			}
		}
	}

	return a.x / a.y;
}

#endif // NOISE_CELLULAR_H
