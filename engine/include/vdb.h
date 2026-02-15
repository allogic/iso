#ifndef VDB_H
#define VDB_H

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

typedef struct cellular_noise_args_t {
  vector4_t offset;
  int32_t type;
  float u;
  float v;
  int32_t axis;
} cellular_noise_args_t;
typedef struct curl_noise_args_t {
  vector4_t offset;
  int32_t type;
  int32_t axis;
  int32_t reserved0;
  int32_t reserved1;
} curl_noise_args_t;
typedef struct fbm_noise_args_t {
  vector4_t offset;
  int32_t type;
  float scale;
  float tile_length;
  float amplitude;
  float lacunarity;
  int32_t octaves;
  int32_t reserved0;
  int32_t reserved1;
} fbm_noise_args_t;
typedef struct gradient_noise_args_t {
  vector4_t offset;
  int32_t type;
  float tile_length;
  int32_t reserved0;
  int32_t reserved1;
} gradient_noise_args_t;
typedef struct perlin_noise_args_t {
  vector4_t offset;
  int32_t type;
  int32_t reserved0;
  int32_t reserved1;
  int32_t reserved2;
} perlin_noise_args_t;
typedef struct simplex_noise_args_t {
  vector4_t offset;
  int32_t type;
  int32_t reserved0;
  int32_t reserved1;
  int32_t reserved2;
} simplex_noise_args_t;

STATIC_ASSERT(ALIGNOF(cellular_noise_args_t) == 4);
STATIC_ASSERT(ALIGNOF(curl_noise_args_t) == 4);
STATIC_ASSERT(ALIGNOF(fbm_noise_args_t) == 4);
STATIC_ASSERT(ALIGNOF(gradient_noise_args_t) == 4);
STATIC_ASSERT(ALIGNOF(perlin_noise_args_t) == 4);
STATIC_ASSERT(ALIGNOF(simplex_noise_args_t) == 4);

typedef struct vdb_terrain_layer_t {
  cellular_noise_args_t cellular_noise_args;
  curl_noise_args_t curl_noise_args;
  fbm_noise_args_t fbm_noise_args;
  gradient_noise_args_t gradient_noise_args;
  perlin_noise_args_t perlin_noise_args;
  simplex_noise_args_t simplex_noise_args;
  int32_t noise_type;
  float scale;
  float weight;
  int32_t reserved0;
} vdb_terrain_layer_t;
typedef struct vdb_cluster_info_t {
  ivector2_t cluster_dim;
  int32_t reserved0;
  int32_t reserved1;
} vdb_cluster_info_t;
typedef struct vdb_chunk_info_t {
  ivector2_t chunk_position;
  ivector2_t chunk_size;
  int32_t visible;
  int32_t reserved0;
  int32_t reserved1;
  int32_t reserved2;
} vdb_chunk_info_t;
typedef struct vdb_chunk_mask_t {
  uint32_t any_px_faces;
  uint32_t any_nx_faces;
  uint32_t any_py_faces;
  uint32_t any_ny_faces;
  uint32_t any_pz_faces;
  uint32_t any_nz_faces;
  uint32_t px_mask[(VDB_CHUNK_SIZE + 2) * (VDB_CHUNK_SIZE + 2)];
  uint32_t nx_mask[(VDB_CHUNK_SIZE + 2) * (VDB_CHUNK_SIZE + 2)];
  uint32_t py_mask[(VDB_CHUNK_SIZE + 2) * (VDB_CHUNK_SIZE + 2)];
  uint32_t ny_mask[(VDB_CHUNK_SIZE + 2) * (VDB_CHUNK_SIZE + 2)];
  uint32_t pz_mask[(VDB_CHUNK_SIZE + 2) * (VDB_CHUNK_SIZE + 2)];
  uint32_t nz_mask[(VDB_CHUNK_SIZE + 2) * (VDB_CHUNK_SIZE + 2)];
} vdb_chunk_mask_t;

STATIC_ASSERT(ALIGNOF(vdb_terrain_layer_t) == 4);
STATIC_ASSERT(ALIGNOF(vdb_cluster_info_t) == 4);
STATIC_ASSERT(ALIGNOF(vdb_chunk_info_t) == 4);
STATIC_ASSERT(ALIGNOF(vdb_chunk_mask_t) == 4);

typedef struct vdb_t {
  buffer_t terrain_layer_buffer;
  buffer_t cluster_info_buffer;
  buffer_t chunk_info_buffer;
  buffer_t chunk_mask_buffer;
  buffer_t chunk_index_buffer;
  VkImage chunk_image;
  VkImageView chunk_image_view;
  VkDeviceMemory chunk_device_memory;
  VkSampler chunk_sampler;
} vdb_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern vdb_t g_vdb;

void vdb_create(void);
void vdb_destroy(void);

int32_t vdb_chunk_position_to_index(ivector2_t chunk_position);
ivector2_t vdb_chunk_index_to_position(int32_t chunk_index);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // VDB_H
