#version 460 core

#extension GL_ARB_shading_language_include : require
#extension GL_EXT_nonuniform_qualifier : require

const float ATLAS_SIZE = 512;
const float ATLAS_TILE_SIZE = 64;
const float ATLAS_LOGICAL_TILE_SIZE = 32;

layout (location = 0) in vec3 input_color;
layout (location = 1) in vec2 input_uv;
layout (location = 2) in flat uint input_tile_index;

layout (location = 0) out vec4 output_color;

layout (binding = 2) uniform sampler2D tile_atlas;

layout (push_constant) uniform push_constant_t {
	vec4 vertex_offset_0;
	vec4 vertex_offset_1;
	vec4 vertex_offset_2;
	vec4 vertex_offset_3;
	vec4 rotation;
	float tile_size;
	float tile_height;
	float inner_scale;
	float outer_scale;
} pc;

vec2 zoomTileUV(vec2 uv, float innerSize, float outerSize)
{
	float scale = innerSize / outerSize;
	float offset = (1.0 - scale) * 0.5;

	return uv * scale + offset;
}

//void main() {
//	float tiles_per_row = ATLAS_SIZE / TILE_SIZE;
//
//	float tile_x = mod(input_tile_index, tiles_per_row);
//	float tile_y = floor(input_tile_index / tiles_per_row);
//
//	vec2 tile_origin = vec2(tile_x, tile_y) * pc.depth_eps;
//	vec2 atlas_uv = (tile_origin + input_uv * pc.depth_eps) / ATLAS_SIZE;
//
//	vec4 color = texture(tile_atlas, atlas_uv);
//
//	if (color.a < 0.01) {
//		discard;
//	}
//
//	output_color = color;
//}

void main() {
	// Number of tiles per row in the atlas (based on 64x64 cells)
	float tiles_per_row = ATLAS_SIZE / ATLAS_TILE_SIZE;

	// Compute tile coordinates
	float tile_x = mod(input_tile_index, tiles_per_row);
	float tile_y = floor(input_tile_index / tiles_per_row);

	// Pixel-space origin of this tile in the atlas
	vec2 tile_origin_px = vec2(tile_x, tile_y) * ATLAS_TILE_SIZE;

	// Zoom UVs into the centered 32x32 region
	vec2 tile_uv = zoomTileUV(
		input_uv,
		pc.inner_scale,
		pc.outer_scale
	);

	// Convert to atlas UVs
	vec2 atlas_uv = (tile_origin_px + tile_uv * ATLAS_TILE_SIZE) / ATLAS_SIZE;

	// Sample atlas
	vec4 color = texture(tile_atlas, atlas_uv);

	// Alpha cutout
	if (color.a < 0.01)
		discard;

	output_color = color;
}