#ifndef MATH_VORONOI_H
#define MATH_VORONOI_H

#include "constants.glsl"
#include "urandom.glsl"

float voronoi(vec2 position, float angle_offset, float radius, float density) {
	vec2 n = floor(position * density);
	vec2 f = fract(position * density);

	float f1 = 8.0;

	for (int j = -1; j <= 1; j++)
	{
		for (int i = -1; i <= 1; i++)
		{
			vec2 g = vec2(i, j);
			vec2 o = urandom2(n + g);

			o = 0.5 + 0.41 * sin(angle_offset + TAU * o);

			vec2 r = (g - f + o) * radius;

			float d = length(r);

			if (d < f1)
			{
				f1 = d;
			}
		}
	}

	return f1;
}

float voronoi(vec3 position, float angle_offset, float radius, float density) {
	vec3 n = floor(position * density);
	vec3 f = fract(position * density);

	float f1 = 8.0;

	for (int k = -1; k <= 1; k++)
	{
		for (int j = -1; j <= 1; j++)
		{
			for (int i = -1; i <= 1; i++)
			{
				vec3 g = vec3(i, j, k);
				vec3 o = urandom3(n + g);

				o = 0.5 + 0.41 * sin(angle_offset + TAU * o);

				vec3 r = (g - f + o) * radius;

				float d = length(r);

				if (d < f1)
				{
					f1 = d;
				}
			}
		}
	}

	return f1;
}

#endif // MATH_VORONOI_H
