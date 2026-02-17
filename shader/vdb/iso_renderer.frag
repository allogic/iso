#version 460 core

#extension GL_ARB_shading_language_include : require
#extension GL_EXT_nonuniform_qualifier : require

const uint ATLAS_SIZE = 512;
const uint TILE_SIZE = 64;

layout (location = 0) in vec3 input_color;
layout (location = 1) in vec2 input_uv;
layout (location = 2) in flat uint input_tile_index;

layout (location = 0) out vec4 output_color;

layout (binding = 2) uniform sampler2D tile_atlas;

void main() {
	float tiles_per_row = ATLAS_SIZE / TILE_SIZE;

	float tile_x = mod(input_tile_index, tiles_per_row);
	float tile_y = floor(input_tile_index / tiles_per_row);

	vec2 tile_origin = vec2(tile_x, tile_y) * TILE_SIZE;
	vec2 atlas_uv = (tile_origin + input_uv * TILE_SIZE) / ATLAS_SIZE;

	vec4 color = texture(tile_atlas, atlas_uv);

	if (color.a < 0.01) {
		discard;
	}

	output_color = color;
}