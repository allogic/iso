#version 460 core

#extension GL_ARB_shading_language_include : require
#extension GL_EXT_nonuniform_qualifier : require

layout (location = 0) in vec2 vertex_position;
layout (location = 1) in vec4 vertex_color;

layout (location = 0) out vec4 output_color;

layout (binding = 0) uniform screen_info_t {
	ivec2 resolution;
} screen_info;

vec2 to_clip(vec2 position, ivec2 screen_resolution) {
	vec2 ndc = (position / screen_resolution) * 2 - 1;

	ndc.y = -ndc.y;

	return ndc;
}

void main() {
	vec4 clip_position = vec4(to_clip(vertex_position, screen_info.resolution), 0, 1);

	output_color = vertex_color;

	gl_Position = clip_position;
}