#ifndef PIPELINE_H
#define PIPELINE_H

typedef enum pipeline_type_t {
  PIPELINE_TYPE_DFLT = 0,
  PIPELINE_TYPE_MESH,
  PIPELINE_TYPE_RAY,
  PIPELINE_TYPE_COMP,
} pipeline_type_t;

typedef struct pipeline_t {
  pipeline_type_t pipeline_type;
  char const *vertex_shader;
  char const *task_shader;
  char const *mesh_shader;
  char const *ray_gen_shader;
  char const *ray_miss_shader;
  char const *ray_closest_hit_shader;
  char const *ray_intersect_shader;
  char const *fragment_shader;
  char const *compute_shader;
  uint32_t enable_blending;
  uint32_t enable_depth_test;
  uint32_t enable_depth_write;
  uint32_t vertex_input_binding_description_count;
  uint32_t vertex_input_attribute_description_count;
  uint32_t push_constant_range_count;
  uint32_t descriptor_pool_size_count;
  uint32_t descriptor_set_layout_binding_count;
  uint32_t descriptor_set_count;
  VkPrimitiveTopology primitive_topology;
  VkPolygonMode polygon_mode;
  VkCullModeFlags cull_mode;
  VkVertexInputBindingDescription *vertex_input_binding_description;
  VkVertexInputAttributeDescription *vertex_input_attribute_description;
  VkPushConstantRange *push_constant_range;
  VkDescriptorPoolSize *descriptor_pool_size;
  VkDescriptorSetLayoutBinding *descriptor_set_layout_binding;
  VkRenderPass *render_pass;
  VkDescriptorPool descriptor_pool;
  VkDescriptorSetLayout descriptor_set_layout_base;
  VkDescriptorSetLayout *descriptor_set_layout;
  VkDescriptorSet *descriptor_set;
  VkPipelineLayout pipeline_layout;
  VkPipeline pipeline_handle;
  VkBuffer sbt_buffer_handle;
  VkDeviceMemory sbt_device_memory;
  VkDeviceAddress sbt_device_address;
  uint32_t ray_gen_group_count;
  uint32_t ray_miss_group_count;
  uint32_t ray_hit_group_count;
  uint32_t callable_group_count;
  VkStridedDeviceAddressRegionKHR ray_gen_region;
  VkStridedDeviceAddressRegionKHR ray_miss_region;
  VkStridedDeviceAddressRegionKHR ray_hit_region;
  VkStridedDeviceAddressRegionKHR callable_region;
} pipeline_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void pipeline_create(pipeline_t *pipeline);
void pipeline_destroy(pipeline_t *pipeline);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PIPELINE_H
