#ifndef DVDB_COMMON_H
#define DVDB_COMMON_H

#define DVDB_IS_VOXEL_SOLID(VOXEL) \
	((VOXEL & DVDB_VOXEL_SOLID_BIT) == DVDB_VOXEL_SOLID_BIT)

#define DVDB_IS_VOXEL_EMPTY(VOXEL) \
	((VOXEL & DVDB_VOXEL_SOLID_BIT) != DVDB_VOXEL_SOLID_BIT)

#define DVDB_SET_VOXEL_SOLID(VOXEL) \
	(VOXEL | DVDB_VOXEL_SOLID_BIT)

const uint DVDB_DIM_X = 8;
const uint DVDB_DIM_Y = 8;
const uint DVDB_DIM_Z = 8;

const ivec3 DVDB_DIMS = ivec3(DVDB_DIM_X, DVDB_DIM_Y, DVDB_DIM_Z);

const uint DVDB_CHUNK_SIZE = 8;
const uint DVDB_CHUNK_COUNT = DVDB_DIM_X * DVDB_DIM_Y * DVDB_DIM_Z;

const uint DVDB_VOXEL_SOLID_BIT = 0x1;

const uint DVDB_EMPTY_VOXEL = 0;

struct dvdb_ray_payload_t {
	vec3 color;
	float t;
};

#endif // DVDB_COMMON_H