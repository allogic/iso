#version 460 core

#extension GL_ARB_shading_language_include : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_shader_16bit_storage : require
#extension GL_EXT_shader_explicit_arithmetic_types_int16 : require

#include "../global.glsl"

#include "../svdb/common.glsl"

layout (location = 0) in uint input_vertex;
layout (location = 1) in ivec4 input_instance;

layout (location = 0) out vec3 output_normal;
layout (location = 1) out vec3 output_color;
layout (location = 2) out vec2 output_uv;
layout (location = 3) out uint output_atlas_id;

layout (binding = 0) uniform camera_info_uniform_t { camera_info_t camera_info; } camera_info_uniform;
layout (binding = 1, std430) readonly buffer svdb_chunk_face_buffer_t { uint chunk_face[]; } svdb_chunk_face_buffer;

void unpack_face(uint f, out uint width, out uint height, out uint face, out uint atlas_id) {
	width    = (f >>  0) & 0x1F;
	height   = (f >>  5) & 0x1F;
	face     = (f >> 10) & 0x7;
	atlas_id = (f >> 13) & 0x7FFFF;
}

void unpack_vertex(uint v, out ivec3 position, out uint corner, out uint face_id) {
	position.x = int((v >>  0) & 0x1F);
	position.y = int((v >>  5) & 0x1F);
	position.z = int((v >> 10) & 0x1F);
	corner     =     (v >> 15) & 0x3;
	face_id    =     (v >> 17) & 0x7FFF;
}

vec3 face_to_normal(uint face) {
	switch (face) {
		case SVDB_FACE_PX: return vec3( 1, 0, 0);
		case SVDB_FACE_PY: return vec3( 0, 1, 0);
		case SVDB_FACE_PZ: return vec3( 0, 0, 1);
		case SVDB_FACE_NX: return vec3(-1, 0, 0);
		case SVDB_FACE_NY: return vec3( 0,-1, 0);
		case SVDB_FACE_NZ: return vec3( 0, 0,-1);
	}

	return vec3(0);
}
vec2 corner_to_uv(uint corner, uint width, uint height) {
	switch (corner) {
		case 0: return vec2(0, height); // TODO: make same as vertex inserts..
		case 1: return vec2(width, height);
		case 2: return vec2(width, 0);
		case 3: return vec2(0, 0);
	}

	return vec2(0);
}

void main() {
	uint v = input_vertex;

	ivec3 position;
	uint corner;
	uint face_id;

	unpack_vertex(v, position, corner, face_id);

	uint f = svdb_chunk_face_buffer.chunk_face[face_id];

	uint width;
	uint height;
	uint face;
	uint atlas_id;

	unpack_face(f, width, height, face, atlas_id);

	vec3 normal = face_to_normal(face);
	vec2 uv = corner_to_uv(corner, width, height);

	vec4 world_position = vec4(input_instance.xyz + position, 1);
	vec4 clip_position  = camera_info_uniform.camera_info.view_projection * world_position;

	output_normal = normal;
	output_color = vec3(1); // TODO
	output_uv = uv;
	output_atlas_id = atlas_id;

	gl_Position = clip_position;
}