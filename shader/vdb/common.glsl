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

const uint CLUSTER_DIM_X = 2;
const uint CLUSTER_DIM_Y = 2;
const uint CLUSTER_DIM_Z = 2;

const ivec3 CLUSTER_SIZE = ivec3(CLUSTER_DIM_X, CLUSTER_DIM_Y, CLUSTER_DIM_Z);

const uint CHUNK_SIZE = 32;
const uint CHUNK_COUNT = CLUSTER_DIM_X * CLUSTER_DIM_Y * CLUSTER_DIM_Z;

const uint VOXEL_IS_SOLID_BIT = 0x1;

const uint EMPTY_VOXEL = 0;

const uint FACE_PX = 0;
const uint FACE_PY = 1;
const uint FACE_PZ = 2;
const uint FACE_NX = 3;
const uint FACE_NY = 4;
const uint FACE_NZ = 5;

const float SURFACE_THRESHOLD = 0.5;

struct chunk_info_t {
	uint vertex_count;
	uint index_count;
};

struct chunk_mask_t {
	uint any_px_faces; // TODO: implement these..
	uint any_py_faces;
	uint any_pz_faces;
	uint any_nx_faces; // TODO: implement these..
	uint any_ny_faces;
	uint any_nz_faces;
	uint opaque_px_mask[CHUNK_SIZE * CHUNK_SIZE];
	uint opaque_py_mask[CHUNK_SIZE * CHUNK_SIZE];
	uint opaque_pz_mask[CHUNK_SIZE * CHUNK_SIZE];
	uint opaque_nx_mask[CHUNK_SIZE * CHUNK_SIZE];
	uint opaque_ny_mask[CHUNK_SIZE * CHUNK_SIZE];
	uint opaque_nz_mask[CHUNK_SIZE * CHUNK_SIZE];
};

struct chunk_vertex_t {
	vec4 position;
	vec4 color;
	vec4 uv;
	uint atlas_id;
	uint reverved0;
	uint reverved1;
	uint reverved2;
};

#endif // VDB_COMMON_H