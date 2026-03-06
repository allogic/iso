#ifndef SVDB_H
#define SVDB_H

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

#define SVDB_VOXEL_SOLID_BIT (0x1)

#define SVDB_EMPTY_VOXEL (0)

#define SVDB_DIM_X (3)
#define SVDB_DIM_Y (3)
#define SVDB_DIM_Z (3)

#define SVDB_CHUNK_SIZE (32)
#define SVDB_CHUNK_COUNT (27)

typedef struct svdb_chunk_vertex_t {
  uint32_t word0;
  uint32_t word1;
  uint32_t word2;
  uint32_t word3;
} svdb_chunk_vertex_t;

STATIC_ASSERT(ALIGN_OF(svdb_chunk_vertex_t) == 4);

typedef uint32_t svdb_chunk_index_t;

typedef struct svdb_world_generator_push_constant_t {
  ivector3_t chunk_position;
  uint32_t chunk_index;
  uint32_t stage;
  uint32_t reserved0;
  uint32_t reserved1;
  uint32_t reserved2;
} svdb_world_generator_push_constant_t;
typedef struct svdb_mask_generator_push_constant_t {
  ivector3_t chunk_position;
  uint32_t chunk_index;
} svdb_mask_generator_push_constant_t;
typedef struct svdb_mesh_generator_push_constant_t {
  ivector3_t chunk_position;
  uint32_t chunk_index;
} svdb_mesh_generator_push_constant_t;
typedef struct svdb_renderer_push_constant_t {
  ivector3_t chunk_position;
  uint32_t chunk_index;
} svdb_renderer_push_constant_t;

STATIC_ASSERT(ALIGN_OF(svdb_world_generator_push_constant_t) == 4);
STATIC_ASSERT(ALIGN_OF(svdb_mask_generator_push_constant_t) == 4);
STATIC_ASSERT(ALIGN_OF(svdb_mesh_generator_push_constant_t) == 4);
STATIC_ASSERT(ALIGN_OF(svdb_renderer_push_constant_t) == 4);

typedef struct svdb_chunk_info_t {
  uint32_t is_dirty;
  uint32_t vertex_count;
  uint32_t index_count;
  uint32_t reserved0;
} svdb_chunk_info_t;
typedef struct svdb_chunk_mask_t {
  uint32_t opaque_px_mask[SVDB_CHUNK_SIZE * SVDB_CHUNK_SIZE];
  uint32_t opaque_py_mask[SVDB_CHUNK_SIZE * SVDB_CHUNK_SIZE];
  uint32_t opaque_pz_mask[SVDB_CHUNK_SIZE * SVDB_CHUNK_SIZE];
  uint32_t opaque_nx_mask[SVDB_CHUNK_SIZE * SVDB_CHUNK_SIZE];
  uint32_t opaque_ny_mask[SVDB_CHUNK_SIZE * SVDB_CHUNK_SIZE];
  uint32_t opaque_nz_mask[SVDB_CHUNK_SIZE * SVDB_CHUNK_SIZE];
} svdb_chunk_mask_t;
typedef struct svdb_select_result_t {
  ivector3_t hit_position;
  uint32_t hit_voxel;
  ivector3_t place_position;
  uint32_t place_voxel;
} svdb_select_result_t;
typedef struct svdb_place_info_t {
  ivector3_t voxel_position;
  uint32_t voxel;
} svdb_place_info_t;
typedef struct svdb_place_result_t {
  uint32_t is_obstructed;
} svdb_place_result_t;

STATIC_ASSERT(ALIGN_OF(svdb_chunk_info_t) == 4);
STATIC_ASSERT(ALIGN_OF(svdb_chunk_mask_t) == 4);
STATIC_ASSERT(ALIGN_OF(svdb_select_result_t) == 4);
STATIC_ASSERT(ALIGN_OF(svdb_place_info_t) == 4);
STATIC_ASSERT(ALIGN_OF(svdb_place_result_t) == 4);

typedef struct svdb_t {
  uint32_t is_dirty;
  uint32_t generate_world;
  uint32_t rebuild_chunk;
  uint32_t chunk_count;
  svdb_select_result_t *select_result;
  svdb_place_info_t *place_info;
  svdb_place_result_t *place_result;
} svdb_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern svdb_t g_svdb;

void svdb_create(uint32_t chunk_count);
void svdb_destroy(void);

void svdb_select_voxel(void);
void svdb_place_voxel(void);

void svdb_generate_world(VkCommandBuffer command_buffer, uint32_t chunk_index);
void svdb_generate_mask(VkCommandBuffer command_buffer, uint32_t chunk_index);
void svdb_generate_mesh(VkCommandBuffer command_buffer, uint32_t chunk_index);

void svdb_draw_mesh(VkCommandBuffer command_buffer, uint32_t chunk_index);

uint32_t svdb_chunk_position_to_index(ivector3_t chunk_position);
ivector3_t svdb_chunk_index_to_position(uint32_t chunk_index);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SVDB_H
