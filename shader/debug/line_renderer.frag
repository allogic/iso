#version 460 core

#extension GL_ARB_shading_language_include : require
#extension GL_EXT_nonuniform_qualifier : require

layout (location = 0) in vec4 input_color;

layout (location = 0) out vec4 output_color;

void main() {
	output_color = input_color;
}