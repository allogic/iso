#ifndef VDB_STATIC_VOXEL_MOD_SINGLE_H
#define VDB_STATIC_VOXEL_MOD_SINGLE_H

#include "../vdb/static_common.glsl"

uint svdb_get_voxel(ivec3 voxel_position) {
	return uint(imageLoad(svdb_voxel_data, voxel_position).r);
}
void svdb_set_voxel(ivec3 voxel_position, uint voxel) {
	imageStore(svdb_voxel_data, voxel_position, uvec4(voxel, 0, 0, 0));
}

#endif // VDB_STATIC_VOXEL_MOD_SINGLE_H