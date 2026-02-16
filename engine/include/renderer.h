#ifndef RENDERER_H
#define RENDERER_H

typedef struct time_info_t {
  float time;
  float delta_time;
} time_info_t;
typedef struct screen_info_t {
  vector2_t resolution;
} screen_info_t;
typedef struct camera_info_t {
  vector3_t position;
  int32_t reserved0;
  matrix4_t view;
  matrix4_t projection;
  matrix4_t view_projection;
  matrix4_t view_projection_inv;
  vector4_t frustum_plane[6];
} camera_info_t;
typedef struct cluster_info_t {
  ivector2_t dimension;
  int32_t reserved0;
  int32_t reserved1;
} cluster_info_t;
typedef struct chunk_info_t {
  ivector2_t chunk_position;
  ivector2_t chunk_size;
  int32_t visible;
  int32_t reserved0;
} chunk_info_t;

STATIC_ASSERT(ALIGNOF(time_info_t) == 4);
STATIC_ASSERT(ALIGNOF(screen_info_t) == 4);
STATIC_ASSERT(ALIGNOF(camera_info_t) == 4);
STATIC_ASSERT(ALIGNOF(cluster_info_t) == 4);
STATIC_ASSERT(ALIGNOF(chunk_info_t) == 4);

typedef struct full_screen_vertex_t {
  vector3_t position;
  int32_t reserved0;
} full_screen_vertex_t;
typedef struct debug_line_vertex_t {
  vector3_t position;
  int32_t reserved0;
  vector4_t color;
} debug_line_vertex_t;

STATIC_ASSERT(ALIGNOF(full_screen_vertex_t) == 4);
STATIC_ASSERT(ALIGNOF(debug_line_vertex_t) == 4);

typedef uint32_t full_screen_index_t;
typedef uint32_t debug_line_index_t;

typedef struct renderer_t {
  int8_t is_dirty;
  int8_t is_debug_enabled;
  int8_t rebuild_world;
} renderer_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern renderer_t g_renderer;

void renderer_create(void);
void renderer_draw(transform_t *transform, camera_t *camera);
void renderer_destroy(void);

void renderer_draw_debug_line(vector3_t from, vector3_t to, vector4_t color);
void renderer_draw_debug_box(vector3_t position, vector3_t size, vector4_t color);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // RENDERER_H
