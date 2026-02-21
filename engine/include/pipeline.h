#ifndef PIPELINE_H
#define PIPELINE_H

typedef enum pipeline_type_t {
  PIPELINE_TYPE_VF = 0,
  PIPELINE_TYPE_TMF,
  PIPELINE_TYPE_C,
} pipeline_type_t;

typedef struct pipeline_t {
  pipeline_type_t pipeline_type;
  char const *vertex_shader;
  char const *task_shader;
  char const *mesh_shader;
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
  VkPrimitiveTopology primitive_topology;
  VkPolygonMode polygon_mode;
  VkVertexInputBindingDescription const *vertex_input_binding_description;
  VkVertexInputAttributeDescription const *vertex_input_attribute_description;
  VkPushConstantRange const *push_constant_range;
  VkDescriptorPoolSize const *descriptor_pool_size;
  VkDescriptorSetLayoutBinding const *descriptor_set_layout_binding;
  VkRenderPass *render_pass;
  VkDescriptorPool descriptor_pool;
  VkDescriptorSetLayout descriptor_set_layout;
  VkDescriptorSet descriptor_set;
  VkPipelineLayout pipeline_layout;
  VkPipeline pipeline_handle;
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
