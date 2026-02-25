#version 460

#extension GL_ARB_shading_language_include : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_ray_tracing : require

layout (location = 0) rayPayloadInEXT vec3 payload;

void main() {
	payload = vec3(0);
}