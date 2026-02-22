#ifndef VDB_COMMON_H
#define VDB_COMMON_H

#define VOXEL_IS_SOLID(VOXEL) \
	((VOXEL & VOXEL_IS_SOLID_BIT) == VOXEL_IS_SOLID_BIT)

#define VOXEL_IS_EMPTY(VOXEL) \
	((VOXEL & VOXEL_IS_SOLID_BIT) != VOXEL_IS_SOLID_BIT)

#define VOXEL_GET_ATLAS_ID(VOXEL) \
	((VOXEL >> 8) & 0xFF)

#define VOXEL_SET_SOLID(VOXEL) \
	(VOXEL | VOXEL_IS_SOLID_BIT)

#define VOXEL_SET_ATLAS_ID(VOXEL, ATLAS_ID) \
	((VOXEL & ~(0xFF << 8)) | ((ATLAS_ID & 0xFF) << 8))

const uint CLUSTER_DIM_X = 1;
const uint CLUSTER_DIM_Y = 1;
const uint CLUSTER_DIM_Z = 1;

const ivec3 CLUSTER_SIZE = ivec3(CLUSTER_DIM_X, CLUSTER_DIM_Y, CLUSTER_DIM_Z);

const uint CHUNK_SIZE = 32;
const uint CHUNK_PAD = 34;
const uint CHUNK_COUNT = CLUSTER_DIM_X * CLUSTER_DIM_Y * CLUSTER_DIM_Z;

const uint VOXEL_IS_SOLID_BIT = 0x1;

const uint EMPTY_VOXEL = 0;

const uint FACE_X = 0;
const uint FACE_Y = 1;
const uint FACE_Z = 2;

const float SURFACE_THRESHOLD = 0.5;

struct chunk_info_t {
	uint vertex_count;
	uint index_count;
};

struct chunk_mask_t {
	uint any_x_faces; // TODO: implement these..
	uint any_y_faces;
	uint any_z_faces;
	uint opaque_x_mask[CHUNK_PAD * CHUNK_PAD];
	uint opaque_y_mask[CHUNK_PAD * CHUNK_PAD];
	uint opaque_z_mask[CHUNK_PAD * CHUNK_PAD];
};

struct chunk_vertex_t {
  vec4 position;
  vec4 color;
};

#endif // VDB_COMMON_H