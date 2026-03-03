#ifndef GLOBAL_H
#define GLOBAL_H

struct camera_info_t {
	vec4 position;
	vec4 direction;
	mat4 view;
	mat4 view_inv;
	mat4 projection;
	mat4 projection_inv;
	mat4 view_projection;
	mat4 view_projection_inv;
	vec4 frustum_plane[6];
};

#endif // GLOBAL_H