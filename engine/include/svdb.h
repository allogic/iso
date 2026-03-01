#ifndef SVDB_H
#define SVDB_H

#define SVDB_DIM_X (1)
#define SVDB_DIM_Y (1)
#define SVDB_DIM_Z (1)

#define SVDB_CHUNK_SIZE (32)
#define SVDB_CHUNK_COUNT (1)

typedef struct svdb_chunk_vertex_t {
  vector4_t position;
  vector4_t color;
  vector4_t uv;
  uint32_t atlas_id;
  uint32_t reserved0;
  uint32_t reserved1;
  uint32_t reserved2;
} svdb_chunk_vertex_t;

STATIC_ASSERT(ALIGNOF(svdb_chunk_vertex_t) == 4);

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

STATIC_ASSERT(ALIGNOF(svdb_world_generator_push_constant_t) == 4);
STATIC_ASSERT(ALIGNOF(svdb_mask_generator_push_constant_t) == 4);
STATIC_ASSERT(ALIGNOF(svdb_mesh_generator_push_constant_t) == 4);
STATIC_ASSERT(ALIGNOF(svdb_renderer_push_constant_t) == 4);

typedef struct svdb_chunk_info_t {
  uint32_t is_dirty;
  uint32_t vertex_count;
  uint32_t index_count;
} svdb_chunk_info_t;
typedef struct svdb_place_info_t {
  ivector3_t voxel_position;
  uint32_t voxel_id;
} svdb_place_info_t;
typedef struct svdb_place_result_t {
  uint32_t is_obstructed;
} svdb_place_result_t;

STATIC_ASSERT(ALIGNOF(svdb_chunk_info_t) == 4);
STATIC_ASSERT(ALIGNOF(svdb_place_info_t) == 4);
STATIC_ASSERT(ALIGNOF(svdb_place_result_t) == 4);

typedef struct svdb_chunk_mask_t {
  uint32_t any_px_faces;
  uint32_t any_py_faces;
  uint32_t any_pz_faces;
  uint32_t any_nx_faces;
  uint32_t any_ny_faces;
  uint32_t any_nz_faces;
  uint32_t opaque_px_mask[SVDB_CHUNK_SIZE * SVDB_CHUNK_SIZE];
  uint32_t opaque_py_mask[SVDB_CHUNK_SIZE * SVDB_CHUNK_SIZE];
  uint32_t opaque_pz_mask[SVDB_CHUNK_SIZE * SVDB_CHUNK_SIZE];
  uint32_t opaque_nx_mask[SVDB_CHUNK_SIZE * SVDB_CHUNK_SIZE];
  uint32_t opaque_ny_mask[SVDB_CHUNK_SIZE * SVDB_CHUNK_SIZE];
  uint32_t opaque_nz_mask[SVDB_CHUNK_SIZE * SVDB_CHUNK_SIZE];
} svdb_chunk_mask_t;

STATIC_ASSERT(ALIGNOF(svdb_chunk_mask_t) == 4);

typedef struct svdb_t {
  svdb_chunk_info_t *chunk_info;
  svdb_place_info_t *place_info;
  svdb_place_result_t *place_result;
} svdb_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern svdb_t g_svdb;

void svdb_create(void);
void svdb_update_descriptors(void);
void svdb_build(void);
void svdb_draw(void);
void svdb_debug(void);
void svdb_destroy(void);

uint32_t svdb_chunk_position_to_index(ivector3_t chunk_position);
ivector3_t svdb_chunk_index_to_position(uint32_t chunk_index);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SVDB_H
