#ifndef VDB_COMMON_H
#define VDB_COMMON_H

#define CHUNK_SIZE (32)
#define CHUNK_COUNT (1)

#define SURFACE_THRESHOLD (0.5)

#define VOXEL_IS_SOLID_BIT (0x1)

#define EMPTY_VOXEL (0)

#define VOXEL_IS_SOLID(VOXEL) \
	((VOXEL & VOXEL_IS_SOLID_BIT) == VOXEL_IS_SOLID_BIT)

#define VOXEL_GET_MATERIAL(VOXEL) \
	((VOXEL >> 8) & 0xFF)

#define VOXEL_SET_SOLID(VOXEL) \
	(VOXEL | VOXEL_IS_SOLID_BIT)

struct payload_t {
	uint chunk_index;
};

#endif // VDB_COMMON_H