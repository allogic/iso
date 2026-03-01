#ifndef SVDB_VOXEL_MOD_H
#define SVDB_VOXEL_MOD_H

#include "../svdb/common.glsl"

uint svdb_get_voxel(ivec3 voxel_position) {
	return uint(imageLoad(svdb_voxel_data, voxel_position).r);
}
void svdb_set_voxel(ivec3 voxel_position, uint voxel) {
	imageStore(svdb_voxel_data, voxel_position, uvec4(voxel, 0, 0, 0));
}

#endif // SVDB_VOXEL_MOD_H