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

STATIC_ASSERT(ALIGNOF(time_info_t) == 4);
STATIC_ASSERT(ALIGNOF(screen_info_t) == 4);
STATIC_ASSERT(ALIGNOF(camera_info_t) == 4);

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
  uint32_t image_index;
  time_info_t *time_info;
  screen_info_t *screen_info;
  mouse_info_t *mouse_info;
  camera_info_t *camera_info;
  VkDescriptorBufferInfo time_info_descriptor_buffer_info;
  VkDescriptorBufferInfo screen_info_descriptor_buffer_info;
  VkDescriptorBufferInfo mouse_info_descriptor_buffer_info;
  VkDescriptorBufferInfo camera_info_descriptor_buffer_info;
} renderer_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern renderer_t g_renderer;

void renderer_create(void);
void renderer_update_descriptors(void);
void renderer_draw(void);
void renderer_debug(void);
void renderer_destroy(void);

void renderer_draw_debug_line(vector3_t from, vector3_t to, vector4_t color);
void renderer_draw_debug_box(vector3_t position, vector3_t size, vector4_t color);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // RENDERER_H
