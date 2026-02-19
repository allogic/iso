#version 460 core

#extension GL_ARB_shading_language_include : require
#extension GL_EXT_nonuniform_qualifier : require

const float ATLAS_SIZE = 512;
const float ATLAS_TILE_SIZE = 32;

layout (location = 0) in vec3 input_color;
layout (location = 1) in vec2 input_uv;
layout (location = 2) in flat uint input_atlas_id;

layout (location = 0) out vec4 output_color;

layout (binding = 4) uniform sampler2D tile_atlas;

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

void main() {
	float tiles_per_row = ATLAS_SIZE / ATLAS_TILE_SIZE;

	float tile_x = mod(input_atlas_id, tiles_per_row);
	float tile_y = floor(input_atlas_id / tiles_per_row);

	vec2 tile_origin = vec2(tile_x, tile_y);
	vec2 atlas_uv = (tile_origin + input_uv) / tiles_per_row;

	// TODO
	// Avoid bleeding..
	//atlas_uv = clamp(atlas_uv, tile_origin / tiles_per_row, (tile_origin + 1.0) / tiles_per_row);

	vec4 color = texture(tile_atlas, atlas_uv);

	if (color.a < 0.01) {
		discard;
	}

	output_color = vec4(input_color / 50.0, 1);
	output_color = color;
}