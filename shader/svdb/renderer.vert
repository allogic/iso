#version 460 core

#extension GL_ARB_shading_language_include : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_shader_16bit_storage : require
#extension GL_EXT_shader_explicit_arithmetic_types_int16 : require

#include "../global.glsl"

#include "../svdb/common.glsl"

layout (location = 0) in uint input_word0;
layout (location = 1) in uint input_word1;
layout (location = 2) in uint input_word2;
layout (location = 3) in uint input_word3;

layout (location = 0) out vec3 output_normal;
layout (location = 1) out vec3 output_color;
layout (location = 2) out vec2 output_uv;
layout (location = 3) out uint output_atlas_id;

layout (binding = 0) uniform camera_info_uniform_t { camera_info_t camera_info; } camera_info_uniform;

layout (push_constant) uniform push_constant_t {
	ivec3 chunk_position;
	uint chunk_index;
} pc;

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

vec2 unpack_uv(uint p) {
	float u = p & 0xFF;
	float v = (p >> 8) & 0xFF;

	return vec2(u, v);
}
vec3 unpack_color(uint p) {
	float r = float((p >> 11) & 0x1F) / 31.0;
	float g = float((p >> 5)  & 0x3F) / 63.0;
	float b = float(p & 0x1F) / 31.0;

	return vec3(r, g, b);
}

void main() {
	float x = float((input_word0 >> 16) & 0xFFFF);
	float y = float(input_word0 & 0xFFFF);
	float z = float((input_word1 >> 16) & 0xFFFF);

	uint packed_uv = input_word1 & 0xFFFF;
	uint packed_color = (input_word2 >> 16) & 0xFFFF;
	uint packed_aux = input_word2 & 0xFFFF;

	uint vertex_face = (packed_aux >> 13) & 0x7;
	uint vertex_atlas_id = (packed_aux >> 7)  & 0x3F;

	vec3 vertex_normal = face_to_normal(vertex_face);
	vec3 vertex_color = unpack_color(packed_color);
	vec2 vertex_uv = unpack_uv(packed_uv);

	vec4 world_position = vec4(pc.chunk_position * SVDB_CHUNK_SIZE, 0) + vec4(x, y, z, 1);
	vec4 clip_position  = camera_info_uniform.camera_info.view_projection * world_position;

	output_normal = vertex_normal;
	output_color = vertex_color;
	output_uv = vertex_uv;
	output_atlas_id = vertex_atlas_id;

	gl_Position = clip_position;
}