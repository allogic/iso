#ifndef VDB_COMMON_H
#define VDB_COMMON_H

#define VDB_CHUNK_SIZE (32)
#define VDB_CLUSTER_DIM_X (1)
#define VDB_CLUSTER_DIM_Y (1)
#define VDB_CHUNK_COUNT (1)

#define VDB_SURFACE_THRESHOLD (0.5)
#define VDB_TERRAIN_LAYER_COUNT (16)

#define VDB_NOISE_TYPE_CELLULAR (0x0)
#define VDB_NOISE_TYPE_CURL (0x1)

#define VDB_CELLULAR_TYPE_0 (0x0)
#define VDB_CELLULAR_TYPE_1 (0x1)

#define VDB_CELLULAR_AXIS_XY (0x0)
#define VDB_CELLULAR_AXIS_XZ (0x1)
#define VDB_CELLULAR_AXIS_YX (0x2)
#define VDB_CELLULAR_AXIS_YZ (0x3)

#define VDB_CURL_TYPE_0 (0x0)
#define VDB_CURL_TYPE_1 (0x1)
#define VDB_CURL_TYPE_2 (0x2)

#define VDB_CURL_AXIS_XY (0x0)
#define VDB_CURL_AXIS_XZ (0x1)
#define VDB_CURL_AXIS_YX (0x2)
#define VDB_CURL_AXIS_YZ (0x3)

#define VDB_VOXEL_IS_SOLID_BIT (0x1)

#define VDB_EMPTY_VOXEL (0)

#define VDB_VOXEL_IS_SOLID(VOXEL) \
	((VOXEL & VDB_VOXEL_IS_SOLID_BIT) == VDB_VOXEL_IS_SOLID_BIT)

#define VDB_VOXEL_GET_MATERIAL(VOXEL) \
	((VOXEL >> 8) & 0xFF)

#define VDB_VOXEL_SET_SOLID(VOXEL) \
	(VOXEL | VDB_VOXEL_IS_SOLID_BIT)

struct cellular_noise_args_t {
	vec4 offset;
	int type;
	float u;
	float v;
	int axis;
};
struct curl_noise_args_t {
	vec4 offset;
	int type;
	int axis;
	int reserved0;
	int reserved1;
};
struct fbm_noise_args_t {
	vec4 offset;
	int type;
	float scale;
	float tile_length;
	float amplitude;
	float lacunarity;
	int octaves;
	int reserved0;
	int reserved1;
};
struct gradient_noise_args_t {
	vec4 offset;
	int type;
	float tile_length;
	int reserved0;
	int reserved1;
};
struct perlin_noise_args_t {
	vec4 offset;
	int type;
	int reserved0;
	int reserved1;
	int reserved2;
};
struct simplex_noise_args_t {
	vec4 offset;
	int type;
	int reserved0;
	int reserved1;
	int reserved2;
};

struct vdb_terrain_layer_t {
	cellular_noise_args_t cellular_noise_args;
	curl_noise_args_t curl_noise_args;
	fbm_noise_args_t fbm_noise_args;
	gradient_noise_args_t gradient_noise_args;
	perlin_noise_args_t perlin_noise_args;
	simplex_noise_args_t simplex_noise_args;
	int noise_type;
	float scale;
	float weight;
	int reserved0;
};
struct vdb_chunk_info_t {
	ivec3 chunk_position;
	int lod;
	vec3 aabb_min;
	int visible;
	vec3 aabb_max;
	int reserved1;
};
struct vdb_chunk_mask_t {
	uint any_px_faces;
	uint any_nx_faces;
	uint any_py_faces;
	uint any_ny_faces;
	uint any_pz_faces;
	uint any_nz_faces;
	uint nx_mask[(VDB_CHUNK_SIZE + 2) * (VDB_CHUNK_SIZE + 2)];
	uint px_mask[(VDB_CHUNK_SIZE + 2) * (VDB_CHUNK_SIZE + 2)];
	uint py_mask[(VDB_CHUNK_SIZE + 2) * (VDB_CHUNK_SIZE + 2)];
	uint ny_mask[(VDB_CHUNK_SIZE + 2) * (VDB_CHUNK_SIZE + 2)];
	uint pz_mask[(VDB_CHUNK_SIZE + 2) * (VDB_CHUNK_SIZE + 2)];
	uint nz_mask[(VDB_CHUNK_SIZE + 2) * (VDB_CHUNK_SIZE + 2)];
};

struct vdb_payload_t {
	uint chunk_index;
};

#endif // VDB_COMMON_H