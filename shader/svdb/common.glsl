#ifndef SVDB_COMMON_H
#define SVDB_COMMON_H

#define SVDB_IS_VOXEL_SOLID(VOXEL) \
	((VOXEL & SVDB_VOXEL_SOLID_BIT) == SVDB_VOXEL_SOLID_BIT)

#define SVDB_IS_VOXEL_EMPTY(VOXEL) \
	((VOXEL & SVDB_VOXEL_SOLID_BIT) != SVDB_VOXEL_SOLID_BIT)

#define SVDB_GET_VOXEL_BLOCK_TYPE(VOXEL) \
	((VOXEL >> 8) & 0xFF)

#define SVDB_SET_VOXEL_SOLID(VOXEL) \
	(VOXEL | SVDB_VOXEL_SOLID_BIT)

#define SVDB_SET_VOXEL_BLOCK_TYPE(VOXEL, BLOCK_TYPE) \
	((VOXEL & ~(0xFF << 8)) | ((BLOCK_TYPE & 0xFF) << 8))

const uint WORST_CASE_GREEDY_MESH_VERTEX_COUNT = 40000;
const uint WORST_CASE_GREEDY_MESH_INDEX_COUNT = 60000;

const uint SVDB_DIM_X = 3;
const uint SVDB_DIM_Y = 3;
const uint SVDB_DIM_Z = 3;

const ivec3 SVDB_DIMS = ivec3(SVDB_DIM_X, SVDB_DIM_Y, SVDB_DIM_Z);

const uint SVDB_CHUNK_SIZE = 32;
const uint SVDB_CHUNK_COUNT = SVDB_DIM_X * SVDB_DIM_Y * SVDB_DIM_Z;

const uint SVDB_VOXEL_SOLID_BIT = 0x1;

const uint SVDB_EMPTY_VOXEL = 0;

const uint SVDB_FACE_PX = 0;
const uint SVDB_FACE_PY = 1;
const uint SVDB_FACE_PZ = 2;
const uint SVDB_FACE_NX = 3;
const uint SVDB_FACE_NY = 4;
const uint SVDB_FACE_NZ = 5;

const float SVDB_SURFACE_THRESHOLD = 0.5;

const uint SVDB_BLOCK_TYPE_DIRT = 0;
const uint SVDB_BLOCK_TYPE_GRASS = 1;
const uint SVDB_BLOCK_TYPE_GRASSY_DIRT = 2;
const uint SVDB_BLOCK_TYPE_STONE = 3;
const uint SVDB_BLOCK_TYPE_COUNT = 4;

const uint SVDB_BLOCK_FACE_NORTH = 0;
const uint SVDB_BLOCK_FACE_SOUTH = 1;
const uint SVDB_BLOCK_FACE_WEST = 2;
const uint SVDB_BLOCK_FACE_EAST = 3;
const uint SVDB_BLOCK_FACE_TOP = 4;
const uint SVDB_BLOCK_FACE_BOTTOM = 5;
const uint SVDB_BLOCK_FACE_COUNT = 6;

struct svdb_cluster_info_t {
	uint chunk_count;
};

struct svdb_mesh_count_t {
	uint vertex_count;
	uint index_count;
};

struct svdb_indirect_draw_t {
	uint index_count;
	uint instance_count;
	uint first_index;
	int vertex_offset;
	uint first_instance;
};

struct svdb_chunk_mask_t {
	uint opaque_px_mask[SVDB_CHUNK_SIZE * SVDB_CHUNK_SIZE];
	uint opaque_py_mask[SVDB_CHUNK_SIZE * SVDB_CHUNK_SIZE];
	uint opaque_pz_mask[SVDB_CHUNK_SIZE * SVDB_CHUNK_SIZE];
	uint opaque_nx_mask[SVDB_CHUNK_SIZE * SVDB_CHUNK_SIZE];
	uint opaque_ny_mask[SVDB_CHUNK_SIZE * SVDB_CHUNK_SIZE];
	uint opaque_nz_mask[SVDB_CHUNK_SIZE * SVDB_CHUNK_SIZE];
};

struct svdb_select_result_t {
	ivec3 hit_position;
	uint hit_voxel;
	ivec3 place_position;
	uint place_voxel;
};

struct svdb_place_info_t {
	ivec3 voxel_position;
	uint voxel;
};

struct svdb_place_result_t {
	uint is_obstructed;
};

struct svdb_chunk_vertex_t {
	uint word0;
	uint word1;
	uint word2;
	uint word3;
};

struct svdb_block_t {
	uint block_type;
	uint block_face_atlas_id[SVDB_BLOCK_FACE_COUNT];
};

#endif // SVDB_COMMON_H