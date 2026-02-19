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
  ivector2_t resolution;
} mouse_info_t;
typedef struct camera_info_t {
  vector2_t position;
  float zoom;
  int32_t reserved0;
} camera_info_t;
typedef struct cluster_info_t {
  ivector2_t dimension;
  int32_t reserved0;
  int32_t reserved1;
} cluster_info_t;
typedef struct place_info_t {
  ivector3_t tile_position;
  uint32_t tile_id;
} place_info_t;
typedef struct chunk_info_t {
  ivector2_t chunk_position;
  ivector2_t chunk_size;
  int32_t visible;
  int32_t reserved0;
} chunk_info_t;
typedef struct place_result_t {
  uint32_t is_obstructed;
} place_result_t;

STATIC_ASSERT(ALIGNOF(time_info_t) == 4);
STATIC_ASSERT(ALIGNOF(screen_info_t) == 4);
STATIC_ASSERT(ALIGNOF(camera_info_t) == 4);
STATIC_ASSERT(ALIGNOF(cluster_info_t) == 4);
STATIC_ASSERT(ALIGNOF(place_info_t) == 4);
STATIC_ASSERT(ALIGNOF(chunk_info_t) == 4);
STATIC_ASSERT(ALIGNOF(place_result_t) == 4);

typedef struct full_screen_vertex_t {
  vector3_t position;
  int32_t reserved0;
} full_screen_vertex_t;
typedef struct debug_line_vertex_t {
  vector2_t position;
  vector4_t color;
} debug_line_vertex_t;

STATIC_ASSERT(ALIGNOF(full_screen_vertex_t) == 4);
STATIC_ASSERT(ALIGNOF(debug_line_vertex_t) == 4);

typedef uint32_t full_screen_index_t;
typedef uint32_t debug_line_index_t;

typedef struct world_generator_push_constant_t {
  uint32_t stage;
} world_generator_push_constant_t;
typedef struct iso_renderer_push_constant_t {
  vector4_t vertex_offset_0;
  vector4_t vertex_offset_1;
  vector4_t vertex_offset_2;
  vector4_t vertex_offset_3;
  vector4_t rotation;
  float tile_size;
  float tile_height;
  float inner_scale;
  float outer_scale;
} iso_renderer_push_constant_t;

STATIC_ASSERT(ALIGNOF(world_generator_push_constant_t) == 4);
STATIC_ASSERT(ALIGNOF(iso_renderer_push_constant_t) == 4);

typedef struct renderer_t {
  int8_t is_dirty;
  int8_t is_debug_enabled;
  int8_t rebuild_world;
  vector4_t vertex_offset_0; // TODO
  vector4_t vertex_offset_1; // TODO
  vector4_t vertex_offset_2; // TODO
  vector4_t vertex_offset_3; // TODO
  vector4_t rotation;        // TODO
  float tile_size;           // TODO
  float tile_height;         // TODO
  float inner_scale;         // TODO
  float outer_scale;         // TODO
} renderer_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern renderer_t g_renderer;

void renderer_create(void);
void renderer_draw(void);
void renderer_destroy(void);

void renderer_draw_debug_line(vector2_t from, vector2_t to, vector4_t color);
void renderer_draw_debug_box(vector2_t position, vector2_t size, vector4_t color);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // RENDERER_H
