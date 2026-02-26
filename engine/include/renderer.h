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
  vector4_t position;
  matrix4_t view;
  matrix4_t view_inv;
  matrix4_t projection;
  matrix4_t projection_inv;
  matrix4_t view_projection;
  matrix4_t view_projection_inv;
  vector4_t frustum_plane[FRUSTUM_PLANE_COUNT];
} camera_info_t;
typedef struct place_info_t {
  ivector3_t voxel_position;
  uint32_t voxel_id;
} place_info_t;
typedef struct place_result_t {
  uint32_t is_obstructed;
} place_result_t;

STATIC_ASSERT(ALIGNOF(time_info_t) == 4);
STATIC_ASSERT(ALIGNOF(screen_info_t) == 4);
STATIC_ASSERT(ALIGNOF(camera_info_t) == 4);
STATIC_ASSERT(ALIGNOF(place_info_t) == 4);
STATIC_ASSERT(ALIGNOF(place_result_t) == 4);

typedef struct full_screen_vertex_t {
  vector4_t position;
} full_screen_vertex_t;
typedef struct debug_line_vertex_t {
  vector4_t position;
  vector4_t color;
} debug_line_vertex_t;

STATIC_ASSERT(ALIGNOF(full_screen_vertex_t) == 4);
STATIC_ASSERT(ALIGNOF(debug_line_vertex_t) == 4);

typedef uint32_t full_screen_index_t;
typedef uint32_t debug_line_index_t;

typedef struct renderer_t {
  uint32_t is_debug_enabled;
  uint32_t rebuild_world;
  pipeline_t static_vdb_voxel_placer_pipeline;
  pipeline_t static_vdb_world_generator_pipeline;
  pipeline_t static_vdb_mask_generator_pipeline;
  pipeline_t static_vdb_mesh_generator_pipeline;
  pipeline_t static_vdb_renderer_pipeline;
  pipeline_t dynamic_vdb_renderer_pipeline;
  pipeline_t debug_line_renderer_pipeline;
  time_info_t *time_info;
  screen_info_t *screen_info;
  mouse_info_t *mouse_info;
  camera_info_t *camera_info;
  place_info_t *place_info;
  place_result_t *place_result;
} renderer_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern renderer_t g_renderer;

void renderer_create(void);
void renderer_draw(void);
void renderer_update(void);
void renderer_destroy(void);

void renderer_draw_debug_line(vector3_t from, vector3_t to, vector4_t color);
void renderer_draw_debug_box(vector3_t position, vector3_t size, vector4_t color);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // RENDERER_H
