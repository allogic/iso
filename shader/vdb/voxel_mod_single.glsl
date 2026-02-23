#ifndef VDB_VOXEL_MOD_SINGLE_H
#define VDB_VOXEL_MOD_SINGLE_H

#include "../vdb/common.glsl"

uint get_voxel(ivec3 voxel_position) {
	return uint(imageLoad(chunk_data, voxel_position).r);
}
void set_voxel(ivec3 voxel_position, uint voxel) {
	imageStore(chunk_data, voxel_position, uvec4(voxel, 0, 0, 0));
}

#endif // VDB_VOXEL_MOD_SINGLE_H