#version 460 core

#extension GL_ARB_shading_language_include : require
#extension GL_EXT_nonuniform_qualifier : require

layout (location = 0) in vec3 input_color;
layout (location = 1) in vec2 input_uv;

layout (location = 0) out vec4 output_color;

layout (binding = 2) uniform usampler2D tile_atlas;

void main() {
	output_color = vec4(input_uv, 0, 1);
}