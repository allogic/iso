#ifndef MATH_GRAD4_H
#define MATH_GRAD4_H

vec4 grad4(float j, vec4 ip) {
	const vec4 ones = vec4(1.0, 1.0, 1.0, -1.0);

	vec4 p;
	vec4 s;

	p.xyz = floor(fract(vec3(j) * ip.xyz) * 7.0) * ip.z - 1.0;
	p.w = 1.5 - dot(abs(p.xyz), ones.xyz);

	s = vec4(lessThan(p, vec4(0.0)));

	p.xyz = p.xyz + (s.xyz * 2.0 - 1.0) * s.www;

	return p;
}

#endif // MATH_GRAD4_H
