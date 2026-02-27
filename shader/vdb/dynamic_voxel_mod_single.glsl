#ifndef VDB_DYNAMIC_VOXEL_MOD_SINGLE_H
#define VDB_DYNAMIC_VOXEL_MOD_SINGLE_H

#include "../vdb/dynamic_common.glsl"

uint dvdb_get_voxel(ivec3 voxel_position) {
	return uint(imageLoad(dvdb_voxel_data, voxel_position).r);
}
void dvdb_set_voxel(ivec3 voxel_position, uint voxel) {
	imageStore(dvdb_voxel_data, voxel_position, uvec4(voxel, 0, 0, 0));
}

#endif // VDB_DYNAMIC_VOXEL_MOD_SINGLE_H