#version 460 core

#extension GL_ARB_shading_language_include : require
#extension GL_EXT_nonuniform_qualifier : require

#include "../svdb/common.glsl"

layout (location = 0) in vec4 vertex_position;
layout (location = 1) in vec4 vertex_color;
layout (location = 2) in vec4 vertex_uv;
layout (location = 3) in uint vertex_atlas_id;

layout (location = 0) out vec4 output_color;
layout (location = 1) out vec2 output_uv;
layout (location = 2) out uint output_atlas_id;

layout (binding = 0) uniform camera_info_t {
	vec4 position;
	vec4 direction;
	mat4 view;
	mat4 view_inv;
	mat4 projection;
	mat4 projection_inv;
	mat4 view_projection;
	mat4 view_projection_inv;
	vec4 frustum_plane[6];
} camera_info;

layout (push_constant) uniform push_constant_t {
	ivec3 chunk_position;
	uint chunk_index;
} pc;

void main() {
	vec4 world_position = vec4(pc.chunk_position * SVDB_CHUNK_SIZE, 0) + vertex_position;
	vec4 clip_position = camera_info.view_projection * world_position;

	output_color = vertex_color;
	output_uv = vec2(vertex_uv.xy);
	output_atlas_id = vertex_atlas_id;

	gl_Position = clip_position;
}