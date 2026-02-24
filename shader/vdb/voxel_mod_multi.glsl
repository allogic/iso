#ifndef VDB_VOXEL_MOD_MULTI_H
#define VDB_VOXEL_MOD_MULTI_H

#include "../vdb/common.glsl"

uint get_chunk_position_to_index(ivec3 chunk_position) {
	return (chunk_position.x) + (chunk_position.y * STATIC_VDB_DIM_X) + (chunk_position.z * STATIC_VDB_DIM_X * STATIC_VDB_DIM_Y);
}

uint get_voxel(ivec3 chunk_position, ivec3 voxel_position) {
	uint chunk_index = get_chunk_position_to_index(chunk_position);

	return uint(imageLoad(voxel_data[chunk_index], voxel_position).r);
}
uint get_voxel_safe(ivec3 chunk_position, ivec3 voxel_position) {
	uint voxel = EMPTY_VOXEL;

	ivec3 chunk_offset = ivec3(
		voxel_position.x < 0 ? -1 : (voxel_position.x >= CHUNK_SIZE ? 1 : 0),
		voxel_position.y < 0 ? -1 : (voxel_position.y >= CHUNK_SIZE ? 1 : 0),
		voxel_position.z < 0 ? -1 : (voxel_position.z >= CHUNK_SIZE ? 1 : 0)
	);

	chunk_position += chunk_offset;
	voxel_position -= chunk_offset * int(CHUNK_SIZE);

	if (all(greaterThanEqual(chunk_position, ivec3(0))) && all(lessThan(chunk_position, STATIC_VDB_DIMS))) {
		
		uint chunk_index = get_chunk_position_to_index(chunk_position);

		voxel = uint(imageLoad(voxel_data[chunk_index], voxel_position).r);
	}

	return voxel;
}

void set_voxel(ivec3 chunk_position, ivec3 voxel_position, uint voxel) {
	uint chunk_index = get_chunk_position_to_index(chunk_position);

	imageStore(voxel_data[chunk_index], voxel_position, uvec4(voxel, 0, 0, 0));
}
void set_voxel_safe(ivec3 chunk_position, ivec3 voxel_position, uint voxel) {
	ivec3 chunk_offset = ivec3(
		voxel_position.x < 0 ? -1 : (voxel_position.x >= CHUNK_SIZE ? 1 : 0),
		voxel_position.y < 0 ? -1 : (voxel_position.y >= CHUNK_SIZE ? 1 : 0),
		voxel_position.z < 0 ? -1 : (voxel_position.z >= CHUNK_SIZE ? 1 : 0)
	);

	chunk_position += chunk_offset;
	voxel_position -= chunk_offset * int(CHUNK_SIZE);

	if (all(greaterThanEqual(chunk_position, ivec3(0))) && all(lessThan(chunk_position, STATIC_VDB_DIMS))) {
		
		uint chunk_index = get_chunk_position_to_index(chunk_position);

		imageStore(voxel_data[chunk_index], voxel_position, uvec4(voxel, 0, 0, 0));
	}
}

#endif // VDB_VOXEL_MOD_MULTI_H