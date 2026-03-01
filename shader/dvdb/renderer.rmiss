#version 460

#extension GL_ARB_shading_language_include : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_ray_tracing : require

#include "../dvdb/common.glsl"

layout (location = 0) rayPayloadInEXT dvdb_ray_payload_t payload;

void main() {
	payload.color = vec3(0);
	payload.t = 1E30;
}