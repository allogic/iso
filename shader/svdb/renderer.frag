#version 460 core

#extension GL_ARB_shading_language_include : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_shader_16bit_storage : require
#extension GL_EXT_shader_explicit_arithmetic_types_int16 : require

#include "../global.glsl"

#include "../svdb/common.glsl"

// TODO: move these into "common.glsl"
const uint ATLAS_SIZE = 512;
const uint ATLAS_TILE_SIZE = 32;

const float TILES_PER_ROW = ATLAS_SIZE / ATLAS_TILE_SIZE;
const float VOXELS_PER_TILE = 1;

layout (location = 0) in vec3 input_normal;
layout (location = 1) in vec3 input_color;
layout (location = 2) in vec2 input_uv;
layout (location = 3) in flat uint input_atlas_id;

layout (location = 0) out vec4 output_color;

layout (binding = 1) uniform sampler2D block_atlas;

void main() {
	vec2 tile_uv = fract(input_uv / VOXELS_PER_TILE);

	float tile_x = mod(float(input_atlas_id), TILES_PER_ROW);
	float tile_y = floor(float(input_atlas_id) / TILES_PER_ROW);

	vec2 tile_origin = vec2(tile_x, tile_y) / TILES_PER_ROW;
	vec2 atlas_uv = tile_origin + tile_uv / TILES_PER_ROW;

	output_color = texture(block_atlas, atlas_uv);
}