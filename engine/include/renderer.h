#ifndef RENDERER_H
#define RENDERER_H

typedef struct time_info_t {
  float time;
  float delta_time;
} time_info_t;
typedef struct screen_info_t {
  ivector2_t resolution;
} screen_info_t;
typedef struct mouse_info_t {
  ivector2_t position;
} mouse_info_t;
typedef struct camera_info_t {
  vector3_t position;
  int32_t reserved0;
  matrix4_t view;
  matrix4_t projection;
  matrix4_t view_projection;
  matrix4_t view_projection_inv;
  vector4_t frustum_plane[FRUSTUM_PLANE_COUNT];
} camera_info_t;
typedef struct cluster_info_t {
  ivector2_t dimension;
  int32_t reserved0;
  int32_t reserved1;
} cluster_info_t;
typedef struct place_info_t {
  ivector3_t voxel_position;
  uint32_t voxel_id;
} place_info_t;
typedef struct chunk_info_t {
  uint32_t vertex_count;
  uint32_t index_count;
} chunk_info_t;
typedef struct chunk_mask_t {
  uint32_t any_px_faces;
  uint32_t any_nx_faces;
  uint32_t any_py_faces;
  uint32_t any_ny_faces;
  uint32_t any_pz_faces;
  uint32_t any_nz_faces;
  uint32_t nx_mask[CHUNK_PAD * CHUNK_PAD];
  uint32_t px_mask[CHUNK_PAD * CHUNK_PAD];
  uint32_t py_mask[CHUNK_PAD * CHUNK_PAD];
  uint32_t ny_mask[CHUNK_PAD * CHUNK_PAD];
  uint32_t pz_mask[CHUNK_PAD * CHUNK_PAD];
  uint32_t nz_mask[CHUNK_PAD * CHUNK_PAD];
} chunk_mask_t;
typedef struct place_result_t {
  uint32_t is_obstructed;
} place_result_t;

STATIC_ASSERT(ALIGNOF(time_info_t) == 4);
STATIC_ASSERT(ALIGNOF(screen_info_t) == 4);
STATIC_ASSERT(ALIGNOF(camera_info_t) == 4);
STATIC_ASSERT(ALIGNOF(cluster_info_t) == 4);
STATIC_ASSERT(ALIGNOF(place_info_t) == 4);
STATIC_ASSERT(ALIGNOF(chunk_info_t) == 4);
STATIC_ASSERT(ALIGNOF(chunk_mask_t) == 4);
STATIC_ASSERT(ALIGNOF(place_result_t) == 4);

typedef struct full_screen_vertex_t {
  vector4_t position;
} full_screen_vertex_t;
typedef struct vdb_chunk_vertex_t {
  vector4_t position;
  vector4_t color;
} vdb_chunk_vertex_t;
typedef struct debug_line_vertex_t {
  vector4_t position;
  vector4_t color;
} debug_line_vertex_t;

STATIC_ASSERT(ALIGNOF(full_screen_vertex_t) == 4);
STATIC_ASSERT(ALIGNOF(vdb_chunk_vertex_t) == 4);
STATIC_ASSERT(ALIGNOF(debug_line_vertex_t) == 4);

typedef uint32_t full_screen_index_t;
typedef uint32_t vdb_chunk_index_t;
typedef uint32_t debug_line_index_t;

typedef struct world_generator_push_constant_t {
  ivector3_t chunk_position;
  uint32_t chunk_index;
  uint32_t stage;
} world_generator_push_constant_t;
typedef struct mask_generator_push_constant_t {
  ivector3_t chunk_position;
  uint32_t chunk_index;
} mask_generator_push_constant_t;
typedef struct greedy_mesher_push_constant_t {
  ivector3_t chunk_position;
  uint32_t chunk_index;
} greedy_mesher_push_constant_t;

STATIC_ASSERT(ALIGNOF(world_generator_push_constant_t) == 4);
STATIC_ASSERT(ALIGNOF(mask_generator_push_constant_t) == 4);
STATIC_ASSERT(ALIGNOF(greedy_mesher_push_constant_t) == 4);

typedef struct renderer_t {
  int8_t is_debug_enabled;
  int8_t rebuild_world;
  time_info_t *time_info;
  screen_info_t *screen_info;
  mouse_info_t *mouse_info;
  camera_info_t *camera_info;
  cluster_info_t *cluster_info;
  chunk_info_t *chunk_info;
  place_info_t *place_info;
  place_result_t *place_result;
  vector4_t vertex_offset_0; // TODO
  vector4_t vertex_offset_1; // TODO
  vector4_t vertex_offset_2; // TODO
  vector4_t vertex_offset_3; // TODO
  vector4_t rotation;        // TODO
} renderer_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern renderer_t g_renderer;

void renderer_create(void);
void renderer_draw(void);
void renderer_destroy(void);

void renderer_draw_debug_line(vector3_t from, vector3_t to, vector4_t color);
void renderer_draw_debug_box(vector3_t position, vector3_t size, vector4_t color);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // RENDERER_H
