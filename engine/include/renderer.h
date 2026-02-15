#ifndef RENDERER_H
#define RENDERER_H

#define DEBUG_LINE_VERTEX_COUNT (0xFFFFF)
#define DEBUG_LINE_INDEX_COUNT (0xFFFFF)

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

STATIC_ASSERT(ALIGNOF(time_info_t) == 4);
STATIC_ASSERT(ALIGNOF(screen_info_t) == 4);
STATIC_ASSERT(ALIGNOF(camera_info_t) == 4);

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
  int32_t image_index;
  int32_t debug_line_vertex_offset;
  int32_t debug_line_index_offset;
  VkSemaphore render_finished_semaphore[SWAPCHAIN_MAX_IMAGE_COUNT];
  VkSemaphore image_available_semaphore;
  VkFence frame_fence;
  VkDescriptorPool vdb_world_generator_descriptor_pool;
  VkDescriptorPool vdb_geom_renderer_descriptor_pool;
  VkDescriptorPool debug_line_descriptor_pool;
  VkDescriptorSetLayout vdb_world_generator_descriptor_set_layout;
  VkDescriptorSetLayout vdb_geom_renderer_descriptor_set_layout;
  VkDescriptorSetLayout debug_line_descriptor_set_layout;
  VkDescriptorSet vdb_world_generator_descriptor_set;
  VkDescriptorSet vdb_geom_renderer_descriptor_set;
  VkDescriptorSet debug_line_descriptor_set;
  VkPipelineLayout vdb_world_generator_pipeline_layout;
  VkPipelineLayout vdb_geom_renderer_pipeline_layout;
  VkPipelineLayout debug_line_pipeline_layout;
  VkPipeline vdb_world_generator_pipeline;
  VkPipeline vdb_geom_renderer_pipeline;
  VkPipeline debug_line_pipeline;
  buffer_t debug_line_vertex_buffer;
  buffer_t debug_line_index_buffer;
  buffer_t time_info_buffer;
  buffer_t screen_info_buffer;
  buffer_t camera_info_buffer;
  buffer_t full_screen_vertex_buffer;
  buffer_t full_screen_index_buffer;
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
