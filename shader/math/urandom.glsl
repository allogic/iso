#ifndef MATH_URANDOM_H
#define MATH_URANDOM_H

const vec4 urandom_scale = vec4(443.897, 441.423, 0.0973, 0.1099);

float urandom(float x) {
	x = fract(x * urandom_scale.x);
	x *= x + 33.33;
	x *= x + x;

	return fract(x);
}

float urandom(vec2 st) {
	vec3 p3 = fract(vec3(st.xyx) * urandom_scale.xyz);

	p3 += dot(p3, p3.yzx + 33.33);

	return fract((p3.x + p3.y) * p3.z);
}

float urandom(vec3 pos) {
	pos = fract(pos * urandom_scale.xyz);
	pos += dot(pos, pos.zyx + 31.32);

	return fract((pos.x + pos.y) * pos.z);
}

float urandom(vec4 pos) {
	pos = fract(pos * urandom_scale);
	pos += dot(pos, pos.wzxy + 33.33);

	return fract((pos.x + pos.y) * (pos.z + pos.w));
}

vec2 urandom2(float p) {
	vec3 p3 = fract(vec3(p) * urandom_scale.xyz);

	p3 += dot(p3, p3.yzx + 19.19);

	return fract((p3.xx + p3.yz) * p3.zy);
}

vec2 urandom2(vec2 p) {
	vec3 p3 = fract(p.xyx * urandom_scale.xyz);

	p3 += dot(p3, p3.yzx + 19.19);

	return fract((p3.xx + p3.yz) * p3.zy);
}

vec2 urandom2(vec3 p3) {
	p3 = fract(p3 * urandom_scale.xyz);
	p3 += dot(p3, p3.yzx + 19.19);

	return fract((p3.xx + p3.yz) * p3.zy);
}

vec3 urandom3(float p) {
	vec3 p3 = fract(vec3(p) * urandom_scale.xyz);

	p3 += dot(p3, p3.yzx + 19.19);

	return fract((p3.xxy + p3.yzz) * p3.zyx);
}

vec3 urandom3(vec2 p) {
	vec3 p3 = fract(vec3(p.xyx) * urandom_scale.xyz);

	p3 += dot(p3, p3.yxz + 19.19);

	return fract((p3.xxy + p3.yzz) * p3.zyx);
}

vec3 urandom3(vec3 p) {
	p = fract(p * urandom_scale.xyz);
	p += dot(p, p.yxz + 19.19);

	return fract((p.xxy + p.yzz) * p.zyx);
}

vec4 urandom4(float p) {
	vec4 p4 = fract(p * urandom_scale);

	p4 += dot(p4, p4.wzxy + 19.19);

	return fract((p4.xxyz + p4.yzzw) * p4.zywx);
}

vec4 urandom4(vec2 p) {
	vec4 p4 = fract(p.xyxy * urandom_scale);

	p4 += dot(p4, p4.wzxy + 19.19);

	return fract((p4.xxyz + p4.yzzw) * p4.zywx);
}

vec4 urandom4(vec3 p) {
	vec4 p4 = fract(p.xyzx * urandom_scale);

	p4 += dot(p4, p4.wzxy + 19.19);

	return fract((p4.xxyz + p4.yzzw) * p4.zywx);
}

vec4 urandom4(vec4 p4) {
	p4 = fract(p4 * urandom_scale);
	p4 += dot(p4, p4.wzxy + 19.19);

	return fract((p4.xxyz + p4.yzzw) * p4.zywx);
}

#endif // MATH_URANDOM_H
