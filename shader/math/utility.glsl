#ifndef MATH_UTILITY_H
#define MATH_UTILITY_H

float remap01(float value, float min_from, float max_from) {
	return (value - min_from) / (max_from - min_from);
}

float remap(float value, float min_from, float max_from, float min_to, float max_to) {
	return min_to + ((value - min_from) / (max_from - min_from)) * (max_to - min_to);
}

float gradiant01(float position, float size) {
	return position / size;
}

float gradiant(float position, float size, float min_to, float max_to) {
	return remap(position / size, 0.0, 1.0, min_to, max_to);
}

float hard_terrace(float value, uint levels) {
	float step_size = 1.0 / float(levels);

	return floor(value / step_size) * step_size;
}

float soft_terrace(float value, uint levels, float sharpness) {
	float step_size = 1.0 / float(levels);
	float level = floor(value / step_size);
	float lower_step = level * step_size;
	float upper_step = (level + 1.0) * step_size;
	float mix_factor = smoothstep(0.0, 1.0, pow((value - lower_step) / step_size, sharpness));

	return mix(lower_step, upper_step, mix_factor);
}

float binarize(float value, float threshold) {
	return (value > threshold) ? 1.0 : 0.0;
}

#endif // MATH_UTILITY_H
