#ifndef SVDB_VOXEL_MODIFIER_H
#define SVDB_VOXEL_MODIFIER_H

uint svdb_get_voxel(ivec3 voxel_position) {
	return uint(imageLoad(svdb_voxel_data, voxel_position + ivec3(1)).r);
}

void svdb_set_voxel(ivec3 voxel_position, uint voxel) {
	imageStore(svdb_voxel_data, voxel_position + ivec3(1), uvec4(voxel, 0, 0, 0));
}

#endif // SVDB_VOXEL_MODIFIER_H