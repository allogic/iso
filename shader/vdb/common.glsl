#ifndef VDB_COMMON_H
#define VDB_COMMON_H

#define VOXEL_IS_SOLID(VOXEL) \
	((VOXEL & VOXEL_IS_SOLID_BIT) == VOXEL_IS_SOLID_BIT)

#define VOXEL_IS_EMPTY(VOXEL) \
	((VOXEL & VOXEL_IS_SOLID_BIT) != VOXEL_IS_SOLID_BIT)

#define VOXEL_GET_BLOCK_TYPE(VOXEL) \
	((VOXEL >> 8) & 0xFF)

#define VOXEL_SET_SOLID(VOXEL) \
	(VOXEL | VOXEL_IS_SOLID_BIT)

#define VOXEL_SET_BLOCK_TYPE(VOXEL, BLOCK_TYPE) \
	((VOXEL & ~(0xFF << 8)) | ((BLOCK_TYPE & 0xFF) << 8))

const uint STATIC_VDB_DIM_X = 1;
const uint STATIC_VDB_DIM_Y = 1;
const uint STATIC_VDB_DIM_Z = 1;

const ivec3 STATIC_VDB_DIMS = ivec3(STATIC_VDB_DIM_X, STATIC_VDB_DIM_Y, STATIC_VDB_DIM_Z);

const uint CHUNK_SIZE = 32;
const uint CHUNK_COUNT = STATIC_VDB_DIM_X * STATIC_VDB_DIM_Y * STATIC_VDB_DIM_Z;

const uint VOXEL_IS_SOLID_BIT = 0x1;

const uint EMPTY_VOXEL = 0;

const uint FACE_PX = 0;
const uint FACE_PY = 1;
const uint FACE_PZ = 2;
const uint FACE_NX = 3;
const uint FACE_NY = 4;
const uint FACE_NZ = 5;

const float SURFACE_THRESHOLD = 0.5;

const uint BLOCK_TYPE_DIRT = 0;
const uint BLOCK_TYPE_GRASS = 1;
const uint BLOCK_TYPE_GRASSY_DIRT = 2;
const uint BLOCK_TYPE_STONE = 3;
const uint BLOCK_TYPE_COUNT = 4;

const uint BLOCK_FACE_NORTH = 0;
const uint BLOCK_FACE_SOUTH = 1;
const uint BLOCK_FACE_WEST = 2;
const uint BLOCK_FACE_EAST = 3;
const uint BLOCK_FACE_TOP = 4;
const uint BLOCK_FACE_BOTTOM = 5;
const uint BLOCK_FACE_COUNT = 6;

struct chunk_info_t {
	uint is_dirty;
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

struct block_t {
	uint block_type;
	uint block_face_atlas_id[BLOCK_FACE_COUNT];
};

#endif // VDB_COMMON_H