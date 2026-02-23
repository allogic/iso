#version 460 core

#extension GL_ARB_shading_language_include : require
#extension GL_EXT_nonuniform_qualifier : require

const uint ATLAS_SIZE = 512;
const uint ATLAS_TILE_SIZE = 32;

const float TILES_PER_ROW = ATLAS_SIZE / ATLAS_TILE_SIZE;
const float VOXELS_PER_TILE = 1;

layout (location = 0) in vec4 input_color;
layout (location = 1) in vec2 input_uv;
layout (location = 2) in flat uint input_atlas_id;

layout (location = 0) out vec4 output_color;

layout (binding = 1) uniform sampler2D texture_atlas;

void main() {
	vec2 tile_uv = fract(input_uv / VOXELS_PER_TILE);

	float tile_x = mod(float(input_atlas_id), TILES_PER_ROW);
	float tile_y = floor(float(input_atlas_id) / TILES_PER_ROW);

	vec2 tile_origin = vec2(tile_x, tile_y) / TILES_PER_ROW;

	vec2 atlas_uv = tile_origin + tile_uv / TILES_PER_ROW;

	output_color = texture(texture_atlas, atlas_uv);
}