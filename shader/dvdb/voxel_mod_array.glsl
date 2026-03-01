#ifndef DVDB_VOXEL_MOD_ARRAY_H
#define DVDB_VOXEL_MOD_ARRAY_H

#include "../dvdb/common.glsl"

uint dvdb_chunk_position_to_index(ivec3 chunk_position) {
	return (chunk_position.x) + (chunk_position.y * DVDB_DIM_X) + (chunk_position.z * DVDB_DIM_X * DVDB_DIM_Y);
}

uint dvdb_get_voxel(ivec3 chunk_position, ivec3 voxel_position) {
	uint chunk_index = dvdb_chunk_position_to_index(chunk_position);

	return uint(imageLoad(dvdb_voxel_data[chunk_index], voxel_position).r);
}
uint dvdb_get_voxel_safe(ivec3 chunk_position, ivec3 voxel_position) {
	uint voxel = DVDB_EMPTY_VOXEL;

	ivec3 chunk_offset = ivec3(
		voxel_position.x < 0 ? -1 : (voxel_position.x >= DVDB_CHUNK_SIZE ? 1 : 0),
		voxel_position.y < 0 ? -1 : (voxel_position.y >= DVDB_CHUNK_SIZE ? 1 : 0),
		voxel_position.z < 0 ? -1 : (voxel_position.z >= DVDB_CHUNK_SIZE ? 1 : 0)
	);

	chunk_position += chunk_offset;
	voxel_position -= chunk_offset * int(DVDB_CHUNK_SIZE);

	if (all(greaterThanEqual(chunk_position, ivec3(0))) && all(lessThan(chunk_position, DVDB_DIMS))) {
		
		uint chunk_index = dvdb_chunk_position_to_index(chunk_position);

		voxel = uint(imageLoad(dvdb_voxel_data[chunk_index], voxel_position).r);
	}

	return voxel;
}

void dvdb_set_voxel(ivec3 chunk_position, ivec3 voxel_position, uint voxel) {
	uint chunk_index = dvdb_chunk_position_to_index(chunk_position);

	imageStore(dvdb_voxel_data[chunk_index], voxel_position, uvec4(voxel, 0, 0, 0));
}
void dvdb_set_voxel_safe(ivec3 chunk_position, ivec3 voxel_position, uint voxel) {
	ivec3 chunk_offset = ivec3(
		voxel_position.x < 0 ? -1 : (voxel_position.x >= DVDB_CHUNK_SIZE ? 1 : 0),
		voxel_position.y < 0 ? -1 : (voxel_position.y >= DVDB_CHUNK_SIZE ? 1 : 0),
		voxel_position.z < 0 ? -1 : (voxel_position.z >= DVDB_CHUNK_SIZE ? 1 : 0)
	);

	chunk_position += chunk_offset;
	voxel_position -= chunk_offset * int(DVDB_CHUNK_SIZE);

	if (all(greaterThanEqual(chunk_position, ivec3(0))) && all(lessThan(chunk_position, DVDB_DIMS))) {
		
		uint chunk_index = dvdb_chunk_position_to_index(chunk_position);

		imageStore(dvdb_voxel_data[chunk_index], voxel_position, uvec4(voxel, 0, 0, 0));
	}
}

#endif // DVDB_VOXEL_MOD_ARRAY_H