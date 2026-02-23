#ifndef VDB_VOXEL_MOD_MULTI_H
#define VDB_VOXEL_MOD_MULTI_H

#include "../vdb/common.glsl"

uint get_voxel(ivec3 chunk_position, ivec3 voxel_position) {
	uint chunk_index = (chunk_position.x) + (chunk_position.y * CLUSTER_DIM_X) + (chunk_position.z * CLUSTER_DIM_X * CLUSTER_DIM_Y);

	return uint(imageLoad(chunk_data[chunk_index], voxel_position).r);
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

	if (all(greaterThanEqual(chunk_position, ivec3(0))) && all(lessThan(chunk_position, CLUSTER_DIMS))) {
		
		uint chunk_index = (chunk_position.x) + (chunk_position.y * CLUSTER_DIM_X) + (chunk_position.z * CLUSTER_DIM_X * CLUSTER_DIM_Y);

		voxel = uint(imageLoad(chunk_data[chunk_index], voxel_position).r);
	}

	return voxel;
}

void set_voxel(ivec3 chunk_position, ivec3 voxel_position, uint voxel) {
	uint chunk_index = (chunk_position.x) + (chunk_position.y * CLUSTER_DIM_X) + (chunk_position.z * CLUSTER_DIM_X * CLUSTER_DIM_Y);

	imageStore(chunk_data[chunk_index], voxel_position, uvec4(voxel, 0, 0, 0));
}
void set_voxel_safe(ivec3 chunk_position, ivec3 voxel_position, uint voxel) {
	ivec3 chunk_offset = ivec3(
		voxel_position.x < 0 ? -1 : (voxel_position.x >= CHUNK_SIZE ? 1 : 0),
		voxel_position.y < 0 ? -1 : (voxel_position.y >= CHUNK_SIZE ? 1 : 0),
		voxel_position.z < 0 ? -1 : (voxel_position.z >= CHUNK_SIZE ? 1 : 0)
	);

	chunk_position += chunk_offset;
	voxel_position -= chunk_offset * int(CHUNK_SIZE);

	if (all(greaterThanEqual(chunk_position, ivec3(0))) && all(lessThan(chunk_position, CLUSTER_DIMS))) {
		
		uint chunk_index = (chunk_position.x) + (chunk_position.y * CLUSTER_DIM_X) + (chunk_position.z * CLUSTER_DIM_X * CLUSTER_DIM_Y);

		imageStore(chunk_data[chunk_index], voxel_position, uvec4(voxel, 0, 0, 0));
	}
}

#endif // VDB_VOXEL_MOD_MULTI_H