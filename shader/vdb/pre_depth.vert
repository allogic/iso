#version 460 core

#extension GL_ARB_shading_language_include : require
#extension GL_EXT_nonuniform_qualifier : require

layout (location = 0) in vec3 vertex_position;

layout (binding = 0) uniform camera_info_t {
	vec3 position;
	float max_ray_distance;
	mat4 view;
	mat4 projection;
	mat4 view_projection;
	mat4 view_projection_inv;
	vec4 frustum_plane[6];
} camera_info;

void main() {
	gl_Position = camera_info.view_projection * vec4(vertex_position, 1.0);
}