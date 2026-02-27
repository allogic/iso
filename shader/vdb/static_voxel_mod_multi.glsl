#ifndef VDB_STATIC_VOXEL_MOD_MULTI_H
#define VDB_STATIC_VOXEL_MOD_MULTI_H

#include "../vdb/static_common.glsl"

uint svdb_get_chunk_position_to_index(ivec3 chunk_position) {
	return (chunk_position.x) + (chunk_position.y * SVDB_DIM_X) + (chunk_position.z * SVDB_DIM_X * SVDB_DIM_Y);
}

uint svdb_get_voxel(ivec3 chunk_position, ivec3 voxel_position) {
	uint chunk_index = svdb_get_chunk_position_to_index(chunk_position);

	return uint(imageLoad(svdb_voxel_data[chunk_index], voxel_position).r);
}
uint svdb_get_voxel_safe(ivec3 chunk_position, ivec3 voxel_position) {
	uint voxel = SVDB_EMPTY_VOXEL;

	ivec3 chunk_offset = ivec3(
		voxel_position.x < 0 ? -1 : (voxel_position.x >= SVDB_CHUNK_SIZE ? 1 : 0),
		voxel_position.y < 0 ? -1 : (voxel_position.y >= SVDB_CHUNK_SIZE ? 1 : 0),
		voxel_position.z < 0 ? -1 : (voxel_position.z >= SVDB_CHUNK_SIZE ? 1 : 0)
	);

	chunk_position += chunk_offset;
	voxel_position -= chunk_offset * int(SVDB_CHUNK_SIZE);

	if (all(greaterThanEqual(chunk_position, ivec3(0))) && all(lessThan(chunk_position, SVDB_DIMS))) {
		
		uint chunk_index = svdb_get_chunk_position_to_index(chunk_position);

		voxel = uint(imageLoad(svdb_voxel_data[chunk_index], voxel_position).r);
	}

	return voxel;
}

void svdb_set_voxel(ivec3 chunk_position, ivec3 voxel_position, uint voxel) {
	uint chunk_index = svdb_get_chunk_position_to_index(chunk_position);

	imageStore(svdb_voxel_data[chunk_index], voxel_position, uvec4(voxel, 0, 0, 0));
}
void svdb_set_voxel_safe(ivec3 chunk_position, ivec3 voxel_position, uint voxel) {
	ivec3 chunk_offset = ivec3(
		voxel_position.x < 0 ? -1 : (voxel_position.x >= SVDB_CHUNK_SIZE ? 1 : 0),
		voxel_position.y < 0 ? -1 : (voxel_position.y >= SVDB_CHUNK_SIZE ? 1 : 0),
		voxel_position.z < 0 ? -1 : (voxel_position.z >= SVDB_CHUNK_SIZE ? 1 : 0)
	);

	chunk_position += chunk_offset;
	voxel_position -= chunk_offset * int(SVDB_CHUNK_SIZE);

	if (all(greaterThanEqual(chunk_position, ivec3(0))) && all(lessThan(chunk_position, SVDB_DIMS))) {
		
		uint chunk_index = svdb_get_chunk_position_to_index(chunk_position);

		imageStore(svdb_voxel_data[chunk_index], voxel_position, uvec4(voxel, 0, 0, 0));
	}
}

#endif // VDB_STATIC_VOXEL_MOD_MULTI_H