#ifndef VDB_COMMON_H
#define VDB_COMMON_H

#define CHUNK_SIZE (32)
#define CHUNK_COUNT (1)
#define CHUNK_PAD (34)

#define SURFACE_THRESHOLD (0.5)

#define VOXEL_IS_SOLID_BIT (0x1)

#define EMPTY_VOXEL (0)

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

struct chunk_info_t {
	uint vertex_count;
	uint index_count;
};

struct chunk_mask_t {
	uint any_px_faces;
	uint any_nx_faces;
	uint any_py_faces;
	uint any_ny_faces;
	uint any_pz_faces;
	uint any_nz_faces;
	uint nx_mask[CHUNK_PAD * CHUNK_PAD];
	uint px_mask[CHUNK_PAD * CHUNK_PAD];
	uint py_mask[CHUNK_PAD * CHUNK_PAD];
	uint ny_mask[CHUNK_PAD * CHUNK_PAD];
	uint pz_mask[CHUNK_PAD * CHUNK_PAD];
	uint nz_mask[CHUNK_PAD * CHUNK_PAD];
};

struct chunk_vertex_t {
  vec4 position;
  vec4 color;
};

#endif // VDB_COMMON_H