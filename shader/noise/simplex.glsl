#ifndef NOISE_SIMPLEX_H
#define NOISE_SIMPLEX_H

#include "../math/grad4.glsl"
#include "../math/mod289.glsl"
#include "../math/permute.glsl"
#include "../math/taylor_inv_sqrt.glsl"

float noise_simplex(vec2 position) {
	const vec4 c = vec4(0.211324865405187, 0.366025403784439, -0.577350269189626, 0.024390243902439);

	vec2 i = floor(position + dot(position, c.yy));
	vec2 x0 = position - i + dot(i, c.xx);
	vec2 i1;

	i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);

	vec4 x12 = x0.xyxy + c.xxzz;

	x12.xy -= i1;

	i = mod289(i);

	vec3 p = permute(permute(i.y + vec3(0.0, i1.y, 1.0)) + i.x + vec3(0.0, i1.x, 1.0));
	vec3 m = max(0.5 - vec3(dot(x0, x0), dot(x12.xy, x12.xy), dot(x12.zw, x12.zw)), 0.0);

	m = m * m;
	m = m * m;

	vec3 x = 2.0 * fract(p * c.www) - 1.0;
	vec3 h = abs(x) - 0.5;
	vec3 ox = floor(x + 0.5);
	vec3 a0 = x - ox;

	m *= 1.79284291400159 - 0.85373472095314 * (a0 * a0 + h * h);

	vec3 g;

	g.x = a0.x * x0.x + h.x * x0.y;
	g.yz = a0.yz * x12.xz + h.yz * x12.yw;

	return 130.0 * dot(m, g);
}

float noise_simplex(vec3 position) {
	const vec2 c = vec2(1.0 / 6.0, 1.0 / 3.0);
	const vec4 d = vec4(0.0, 0.5, 1.0, 2.0);

	vec3 i = floor(position + dot(position, c.yyy));
	vec3 x0 = position - i + dot(i, c.xxx);
	vec3 g = step(x0.yzx, x0.xyz);
	vec3 l = 1.0 - g;
	vec3 i1 = min(g.xyz, l.zxy);
	vec3 i2 = max(g.xyz, l.zxy);
	vec3 x1 = x0 - i1 + c.xxx;
	vec3 x2 = x0 - i2 + c.yyy;
	vec3 x3 = x0 - d.yyy;

	i = mod289(i);

	vec4 p = permute(permute(permute(i.z + vec4(0.0, i1.z, i2.z, 1.0)) + i.y + vec4(0.0, i1.y, i2.y, 1.0)) + i.x + vec4(0.0, i1.x, i2.x, 1.0));
	float n_ = 0.142857142857;
	vec3 ns = n_ * d.wyz - d.xzx;
	vec4 j = p - 49.0 * floor(p * ns.z * ns.z);
	vec4 xx = floor(j * ns.z);
	vec4 yy = floor(j - 7.0 * xx);
	vec4 x = xx * ns.x + ns.yyyy;
	vec4 y = yy * ns.x + ns.yyyy;
	vec4 h = 1.0 - abs(x) - abs(y);

	vec4 b0 = vec4(x.xy, y.xy);
	vec4 b1 = vec4(x.zw, y.zw);

	vec4 s0 = floor(b0) * 2.0 + 1.0;
	vec4 s1 = floor(b1) * 2.0 + 1.0;
	vec4 sh = -step(h, vec4(0.0));

	vec4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
	vec4 a1 = b1.xzyw + s1.xzyw * sh.zzww;

	vec3 p0 = vec3(a0.xy, h.x);
	vec3 p1 = vec3(a0.zw, h.y);
	vec3 p2 = vec3(a1.xy, h.z);
	vec3 p3 = vec3(a1.zw, h.w);

	vec4 norm = taylor_inv_sqrt(vec4(dot(p0, p0), dot(p1, p1), dot(p2, p2), dot(p3, p3)));

	p0 *= norm.x;
	p1 *= norm.y;
	p2 *= norm.z;
	p3 *= norm.w;

	vec4 m = max(0.6 - vec4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), 0.0);

	m = m * m;

	return 42.0 * dot(m * m, vec4(dot(p0, x0), dot(p1, x1), dot(p2, x2), dot(p3, x3)));
}

float noise_simplex(vec4 position) {
	const vec4 c = vec4(0.138196601125011, 0.276393202250021, 0.414589803375032, -0.447213595499958);

	vec4 i = floor(position + dot(position, vec4(.309016994374947451)));
	vec4 x0 = position - i + dot(i, c.xxxx);
	vec4 i0;
	vec3 is_x = step(x0.yzw, x0.xxx);
	vec3 is_yz = step(x0.zww, x0.yyz);

	i0.x = is_x.x + is_x.y + is_x.z;
	i0.yzw = 1.0 - is_x;
	i0.y += is_yz.x + is_yz.y;
	i0.zw += 1.0 - is_yz.xy;
	i0.z += is_yz.z;
	i0.w += 1.0 - is_yz.z;

	vec4 i3 = clamp(i0, 0.0, 1.0);
	vec4 i2 = clamp(i0 - 1.0, 0.0, 1.0);
	vec4 i1 = clamp(i0 - 2.0, 0.0, 1.0);

	vec4 x1 = x0 - i1 + c.xxxx;
	vec4 x2 = x0 - i2 + c.yyyy;
	vec4 x3 = x0 - i3 + c.zzzz;
	vec4 x4 = x0 + c.wwww;

	i = mod289(i);

	float j0 = permute(permute(permute(permute(i.w) + i.z) + i.y) + i.x);
	vec4 j1 = permute(permute(permute(permute(i.w + vec4(i1.w, i2.w, i3.w, 1.0)) + i.z + vec4(i1.z, i2.z, i3.z, 1.0)) + i.y + vec4(i1.y, i2.y, i3.y, 1.0)) + i.x + vec4(i1.x, i2.x, i3.x, 1.0));
	vec4 ip = vec4(1.0 / 294.0, 1.0 / 49.0, 1.0 / 7.0, 0.0);

	vec4 p0 = grad4(j0, ip);
	vec4 p1 = grad4(j1.x, ip);
	vec4 p2 = grad4(j1.y, ip);
	vec4 p3 = grad4(j1.z, ip);
	vec4 p4 = grad4(j1.w, ip);

	vec4 norm = taylor_inv_sqrt(vec4(dot(p0, p0), dot(p1, p1), dot(p2, p2), dot(p3, p3)));

	p0 *= norm.x;
	p1 *= norm.y;
	p2 *= norm.z;
	p3 *= norm.w;
	p4 *= taylor_inv_sqrt(dot(p4, p4));

	vec3 m0 = max(0.6 - vec3(dot(x0, x0), dot(x1, x1), dot(x2, x2)), 0.0);
	vec2 m1 = max(0.6 - vec2(dot(x3, x3), dot(x4, x4)), 0.0);

	m0 = m0 * m0;
	m1 = m1 * m1;

	return 49.0 * (dot(m0 * m0, vec3(dot(p0, x0), dot(p1, x1), dot(p2, x2))) + dot(m1 * m1, vec2(dot(p3, x3), dot(p4, x4))));
}

vec2 noise_simplex2(vec2 position) {
	float s = noise_simplex(vec2(position));
	float s1 = noise_simplex(vec2(position.y - 19.1, position.x + 47.2));

	return vec2(s, s1);
}

vec3 noise_simplex3(vec3 position) {
	float s = noise_simplex(vec3(position));
	float s1 = noise_simplex(vec3(position.y - 19.1, position.z + 33.4, position.x + 47.2));
	float s2 = noise_simplex(vec3(position.z + 74.2, position.x - 124.5, position.y + 99.4));

	return vec3(s, s1, s2);
}

vec3 noise_simplex4(vec4 position) {
	float s = noise_simplex(vec4(position));
	float s1 = noise_simplex(vec4(position.y - 19.1, position.z + 33.4, position.x + 47.2, position.w));
	float s2 = noise_simplex(vec4(position.z + 74.2, position.x - 124.5, position.y + 99.4, position.w));

	return vec3(s, s1, s2);
}

#endif // NOISE_SIMPLEX_H
