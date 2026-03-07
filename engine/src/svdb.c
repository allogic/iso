#include <pch.h>

//    32 ^ 3 sides        = 32768
// 32768 / 2 checkerboard = 16384
// 16384 x 6 faces        = 98304
// 98304 x 4 vertices     = 393216
// 98304 x 6 indices      = 589824
#define WORST_CASE_GREEDY_MESH_FACE_COUNT (32767)
#define WORST_CASE_GREEDY_MESH_VERTEX_COUNT (40000)
#define WORST_CASE_GREEDY_MESH_INDEX_COUNT (60000)

static void svdb_create_cluster_info_buffer(void);
static void svdb_create_chunk_mask_buffer(void);
static void svdb_create_chunk_instance_buffer(void);
static void svdb_create_chunk_face_buffer(void);
static void svdb_create_chunk_vertex_buffer(void);
static void svdb_create_chunk_index_buffer(void);
static void svdb_create_mesh_count_buffer(void);
static void svdb_create_indirect_draw_buffer(void);
static void svdb_create_select_result_buffer(void);
static void svdb_create_place_info_buffer(void);
static void svdb_create_place_result_buffer(void);
static void svdb_create_block_buffer(void);

static void svdb_create_chunk_voxel_image(void);
static void svdb_create_block_atlas_image(void);

static void svdb_update_voxel_selector_descriptor_set(void);
static void svdb_update_voxel_placer_descriptor_set(void);
static void svdb_update_world_generator_descriptor_set(void);
static void svdb_update_mask_generator_descriptor_set(void);
static void svdb_update_mesh_generator_descriptor_set(void);
static void svdb_update_idraw_generator_descriptor_set(void);
static void svdb_update_renderer_descriptor_set(void);

static void svdb_destroy_buffer(void);
static void svdb_destroy_image(void);

static VkVertexInputBindingDescription s_vertex_input_binding_description[] = {
  {
    .binding = 0,
    .stride = sizeof(svdb_chunk_vertex_t),
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
  },
  {
    .binding = 1,
    .stride = sizeof(svdb_chunk_instance_t),
    .inputRate = VK_VERTEX_INPUT_RATE_INSTANCE,
  },
};

static VkVertexInputAttributeDescription s_vertex_input_attribute_description[] = {
  {
    .location = 0,
    .binding = 0,
    .format = VK_FORMAT_R32_UINT,
    .offset = 0,
  },
  {
    .location = 1,
    .binding = 1,
    .format = VK_FORMAT_R32G32B32A32_SINT,
    .offset = 0,
  },
};

static VkDescriptorPoolSize s_voxel_selector_descriptor_pool_size[] = {
  {
    .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
  },
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    .descriptorCount = 1,
  },
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 1,
  },
};
static VkDescriptorPoolSize s_voxel_placer_descriptor_pool_size[] = {
  {
    .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
  },
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    .descriptorCount = 1,
  },
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 1,
  },
};
static VkDescriptorPoolSize s_world_generator_descriptor_pool_size[] = {
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 1,
  },
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    .descriptorCount = 1,
  },
};
static VkDescriptorPoolSize s_mask_generator_descriptor_pool_size[] = {
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    .descriptorCount = 1,
  },
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 1,
  },
};
static VkDescriptorPoolSize s_mesh_generator_descriptor_pool_size[] = {
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 6,
  },
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    .descriptorCount = 1,
  },
};
static VkDescriptorPoolSize s_idraw_generator_descriptor_pool_size[] = {
  {
    .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
  },
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 2,
  },
};
static VkDescriptorPoolSize s_renderer_descriptor_pool_size[] = {
  {
    .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
  },
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 1,
  },
  {
    .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    .descriptorCount = 1,
  },
};

static VkDescriptorSetLayoutBinding s_voxel_selector_descriptor_set_layout_binding[] = {
  {
    .binding = 0,
    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .pImmutableSamplers = 0,
  },
  {
    .binding = 1,
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .pImmutableSamplers = 0,
  },
  {
    .binding = 2,
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .pImmutableSamplers = 0,
  },
};
static VkDescriptorSetLayoutBinding s_voxel_placer_descriptor_set_layout_binding[] = {
  {
    .binding = 0,
    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .pImmutableSamplers = 0,
  },
  {
    .binding = 1,
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .pImmutableSamplers = 0,
  },
  {
    .binding = 2,
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .pImmutableSamplers = 0,
  },
};
static VkDescriptorSetLayoutBinding s_world_generator_descriptor_set_layout_binding[] = {
  {
    .binding = 0,
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .pImmutableSamplers = 0,
  },
  {
    .binding = 1,
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .pImmutableSamplers = 0,
  },
};
static VkDescriptorSetLayoutBinding s_mask_generator_descriptor_set_layout_binding[] = {
  {
    .binding = 0,
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .pImmutableSamplers = 0,
  },
  {
    .binding = 1,
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .pImmutableSamplers = 0,
  },
};
static VkDescriptorSetLayoutBinding s_mesh_generator_descriptor_set_layout_binding[] = {
  {
    .binding = 0,
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .pImmutableSamplers = 0,
  },
  {
    .binding = 1,
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .pImmutableSamplers = 0,
  },
  {
    .binding = 2,
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .pImmutableSamplers = 0,
  },
  {
    .binding = 3,
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .pImmutableSamplers = 0,
  },
  {
    .binding = 4,
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .pImmutableSamplers = 0,
  },
  {
    .binding = 5,
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .pImmutableSamplers = 0,
  },
  {
    .binding = 6,
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .pImmutableSamplers = 0,
  },
};
static VkDescriptorSetLayoutBinding s_idraw_generator_descriptor_set_layout_binding[] = {
  {
    .binding = 0,
    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .pImmutableSamplers = 0,
  },
  {
    .binding = 1,
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .pImmutableSamplers = 0,
  },
  {
    .binding = 2,
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .pImmutableSamplers = 0,
  },
};
static VkDescriptorSetLayoutBinding s_renderer_descriptor_set_layout_binding[] = {
  {
    .binding = 0,
    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    .pImmutableSamplers = 0,
  },
  {
    .binding = 1,
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    .pImmutableSamplers = 0,
  },
  {
    .binding = 2,
    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
    .pImmutableSamplers = 0,
  },
};

static VkPushConstantRange s_world_generator_push_constant_range[] = {
  {
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .offset = 0,
    .size = sizeof(svdb_world_generator_push_constant_t),
  },
};
static VkPushConstantRange s_mask_generator_push_constant_range[] = {
  {
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .offset = 0,
    .size = sizeof(svdb_mask_generator_push_constant_t),
  },
};
static VkPushConstantRange s_mesh_generator_push_constant_range[] = {
  {
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .offset = 0,
    .size = sizeof(svdb_mesh_generator_push_constant_t),
  },
};

static buffer_t s_cluster_info_buffer = {
  .size = sizeof(svdb_cluster_info_t),
  .buffer_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
};
static buffer_t s_chunk_mask_buffer = {
  .buffer_usage_flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
};
static buffer_t s_chunk_instance_buffer = {
  .buffer_usage_flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
};
static buffer_t s_chunk_face_buffer = {
  .buffer_usage_flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
};
static buffer_t s_chunk_vertex_buffer = {
  .buffer_usage_flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
};
static buffer_t s_chunk_index_buffer = {
  .buffer_usage_flags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
};
static buffer_t s_mesh_count_buffer = {
  .buffer_usage_flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
};
static buffer_t s_indirect_draw_buffer = {
  .buffer_usage_flags = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
};
static buffer_t s_select_result_buffer = {
  .size = sizeof(svdb_select_result_t),
  .buffer_usage_flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
};
static buffer_t s_place_info_buffer = {
  .size = sizeof(svdb_place_info_t),
  .buffer_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
};
static buffer_t s_place_result_buffer = {
  .size = sizeof(svdb_place_result_t),
  .buffer_usage_flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
};
static buffer_t s_block_buffer = {
  .host_data = g_block,
  .size = sizeof(g_block),
  .buffer_usage_flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
};

static image_t s_chunk_voxel_image = {
  .width = SVDB_CHUNK_SIZE + 2,
  .height = SVDB_CHUNK_SIZE + 2,
  .depth = SVDB_CHUNK_SIZE + 2,
  .channel = 1,
  .element_size = sizeof(uint32_t),
  .format = VK_FORMAT_R32_UINT,
  .filter = VK_FILTER_NEAREST,
  .image_usage_flags = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
  .image_type = VK_IMAGE_TYPE_3D,
  .image_view_type = VK_IMAGE_VIEW_TYPE_3D,
  .image_aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT,
  .image_tiling = VK_IMAGE_TILING_OPTIMAL,
};
static image_t s_block_atlas_image = {
  .width = 512,
  .height = 512,
  .depth = 1,
  .channel = 4,
  .element_size = sizeof(uint8_t),
  .format = VK_FORMAT_R8G8B8A8_UNORM,
  .filter = VK_FILTER_NEAREST,
  .image_usage_flags = VK_IMAGE_USAGE_SAMPLED_BIT,
  .image_type = VK_IMAGE_TYPE_2D,
  .image_view_type = VK_IMAGE_VIEW_TYPE_2D,
  .image_aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT,
  .image_tiling = VK_IMAGE_TILING_OPTIMAL,
};

static VkDescriptorBufferInfo s_cluster_info_descriptor_buffer_info = {0};
static VkDescriptorBufferInfo s_chunk_mask_descriptor_buffer_info = {0};
static VkDescriptorBufferInfo s_chunk_instance_descriptor_buffer_info = {0};
static VkDescriptorBufferInfo s_chunk_face_descriptor_buffer_info = {0};
static VkDescriptorBufferInfo s_chunk_vertex_descriptor_buffer_info = {0};
static VkDescriptorBufferInfo s_chunk_index_descriptor_buffer_info = {0};
static VkDescriptorBufferInfo s_mesh_count_descriptor_buffer_info = {0};
static VkDescriptorBufferInfo s_indirect_draw_descriptor_buffer_info = {0};
static VkDescriptorBufferInfo s_select_result_descriptor_buffer_info = {0};
static VkDescriptorBufferInfo s_place_info_descriptor_buffer_info = {0};
static VkDescriptorBufferInfo s_place_result_descriptor_buffer_info = {0};
static VkDescriptorBufferInfo s_block_descriptor_buffer_info = {0};

static VkDescriptorImageInfo s_chunk_voxel_descriptor_image_info = {0};
static VkDescriptorImageInfo s_block_atlas_descriptor_image_info = {0};

static pipeline_t s_voxel_selector_pipeline = {
  .pipeline_type = PIPELINE_TYPE_COMP,
  .compute_shader = ROOT_DIR "/shader/svdb/voxel_selector.comp.spv",
  .descriptor_pool_size = s_voxel_selector_descriptor_pool_size,
  .descriptor_pool_size_count = ARRAY_COUNT(s_voxel_selector_descriptor_pool_size),
  .descriptor_set_layout_binding = s_voxel_selector_descriptor_set_layout_binding,
  .descriptor_set_layout_binding_count = ARRAY_COUNT(s_voxel_selector_descriptor_set_layout_binding),
  .descriptor_set_count = 1,
};
static pipeline_t s_voxel_placer_pipeline = {
  .pipeline_type = PIPELINE_TYPE_COMP,
  .compute_shader = ROOT_DIR "/shader/svdb/voxel_placer.comp.spv",
  .descriptor_pool_size = s_voxel_placer_descriptor_pool_size,
  .descriptor_pool_size_count = ARRAY_COUNT(s_voxel_placer_descriptor_pool_size),
  .descriptor_set_layout_binding = s_voxel_placer_descriptor_set_layout_binding,
  .descriptor_set_layout_binding_count = ARRAY_COUNT(s_voxel_placer_descriptor_set_layout_binding),
  .descriptor_set_count = 1,
};
static pipeline_t s_world_generator_pipeline = {
  .pipeline_type = PIPELINE_TYPE_COMP,
  .compute_shader = ROOT_DIR "/shader/svdb/world_generator.comp.spv",
  .push_constant_range = s_world_generator_push_constant_range,
  .push_constant_range_count = ARRAY_COUNT(s_world_generator_push_constant_range),
  .descriptor_pool_size = s_world_generator_descriptor_pool_size,
  .descriptor_pool_size_count = ARRAY_COUNT(s_world_generator_descriptor_pool_size),
  .descriptor_set_layout_binding = s_world_generator_descriptor_set_layout_binding,
  .descriptor_set_layout_binding_count = ARRAY_COUNT(s_world_generator_descriptor_set_layout_binding),
  .descriptor_set_count = 1,
};
static pipeline_t s_mask_generator_pipeline = {
  .pipeline_type = PIPELINE_TYPE_COMP,
  .compute_shader = ROOT_DIR "/shader/svdb/mask_generator.comp.spv",
  .push_constant_range = s_mask_generator_push_constant_range,
  .push_constant_range_count = ARRAY_COUNT(s_mask_generator_push_constant_range),
  .descriptor_pool_size = s_mask_generator_descriptor_pool_size,
  .descriptor_pool_size_count = ARRAY_COUNT(s_mask_generator_descriptor_pool_size),
  .descriptor_set_layout_binding = s_mask_generator_descriptor_set_layout_binding,
  .descriptor_set_layout_binding_count = ARRAY_COUNT(s_mask_generator_descriptor_set_layout_binding),
  .descriptor_set_count = 1,
};
static pipeline_t s_mesh_generator_pipeline = {
  .pipeline_type = PIPELINE_TYPE_COMP,
  .compute_shader = ROOT_DIR "/shader/svdb/mesh_generator.comp.spv",
  .push_constant_range = s_mesh_generator_push_constant_range,
  .push_constant_range_count = ARRAY_COUNT(s_mesh_generator_push_constant_range),
  .descriptor_pool_size = s_mesh_generator_descriptor_pool_size,
  .descriptor_pool_size_count = ARRAY_COUNT(s_mesh_generator_descriptor_pool_size),
  .descriptor_set_layout_binding = s_mesh_generator_descriptor_set_layout_binding,
  .descriptor_set_layout_binding_count = ARRAY_COUNT(s_mesh_generator_descriptor_set_layout_binding),
  .descriptor_set_count = 1,
};
static pipeline_t s_idraw_generator_pipeline = {
  .pipeline_type = PIPELINE_TYPE_COMP,
  .compute_shader = ROOT_DIR "/shader/svdb/idraw_generator.comp.spv",
  .descriptor_pool_size = s_idraw_generator_descriptor_pool_size,
  .descriptor_pool_size_count = ARRAY_COUNT(s_idraw_generator_descriptor_pool_size),
  .descriptor_set_layout_binding = s_idraw_generator_descriptor_set_layout_binding,
  .descriptor_set_layout_binding_count = ARRAY_COUNT(s_idraw_generator_descriptor_set_layout_binding),
  .descriptor_set_count = 1,
};
static pipeline_t s_renderer_pipeline = {
  .pipeline_type = PIPELINE_TYPE_DFLT,
  .vertex_shader = ROOT_DIR "/shader/svdb/renderer.vert.spv",
  .fragment_shader = ROOT_DIR "/shader/svdb/renderer.frag.spv",
  .enable_blending = 1,
  .enable_depth_test = 1,
  .enable_depth_write = 1,
  .primitive_topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
  .polygon_mode = VK_POLYGON_MODE_FILL,
  .cull_mode = VK_CULL_MODE_BACK_BIT,
  .vertex_input_binding_description = s_vertex_input_binding_description,
  .vertex_input_binding_description_count = ARRAY_COUNT(s_vertex_input_binding_description),
  .vertex_input_attribute_description = s_vertex_input_attribute_description,
  .vertex_input_attribute_description_count = ARRAY_COUNT(s_vertex_input_attribute_description),
  .descriptor_pool_size = s_renderer_descriptor_pool_size,
  .descriptor_pool_size_count = ARRAY_COUNT(s_renderer_descriptor_pool_size),
  .descriptor_set_layout_binding = s_renderer_descriptor_set_layout_binding,
  .descriptor_set_layout_binding_count = ARRAY_COUNT(s_renderer_descriptor_set_layout_binding),
  .descriptor_set_count = 1,
  .render_pass = &g_renderpass,
};

svdb_t g_svdb = {0};

void svdb_create(uint32_t chunk_count) {
  g_svdb.chunk_count = chunk_count;

  svdb_create_cluster_info_buffer();
  svdb_create_chunk_mask_buffer();
  svdb_create_chunk_instance_buffer();
  svdb_create_chunk_face_buffer();
  svdb_create_chunk_vertex_buffer();
  svdb_create_chunk_index_buffer();
  svdb_create_mesh_count_buffer();
  svdb_create_indirect_draw_buffer();
  svdb_create_select_result_buffer();
  svdb_create_place_info_buffer();
  svdb_create_place_result_buffer();
  svdb_create_block_buffer();

  svdb_create_chunk_voxel_image();
  svdb_create_block_atlas_image();

  pipeline_create(&s_voxel_selector_pipeline);
  pipeline_create(&s_voxel_placer_pipeline);
  pipeline_create(&s_world_generator_pipeline);
  pipeline_create(&s_mask_generator_pipeline);
  pipeline_create(&s_mesh_generator_pipeline);
  pipeline_create(&s_idraw_generator_pipeline);
  pipeline_create(&s_renderer_pipeline);

  svdb_update_voxel_selector_descriptor_set();
  svdb_update_voxel_placer_descriptor_set();
  svdb_update_world_generator_descriptor_set();
  svdb_update_mask_generator_descriptor_set();
  svdb_update_mesh_generator_descriptor_set();
  svdb_update_idraw_generator_descriptor_set();
  svdb_update_renderer_descriptor_set();
}
void svdb_destroy(void) {
  pipeline_destroy(&s_voxel_selector_pipeline);
  pipeline_destroy(&s_voxel_placer_pipeline);
  pipeline_destroy(&s_world_generator_pipeline);
  pipeline_destroy(&s_mask_generator_pipeline);
  pipeline_destroy(&s_mesh_generator_pipeline);
  pipeline_destroy(&s_idraw_generator_pipeline);
  pipeline_destroy(&s_renderer_pipeline);

  svdb_destroy_buffer();
  svdb_destroy_image();
}

void svdb_select_voxel(void) {
  vkCmdBindPipeline(g_renderer.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_voxel_selector_pipeline.pipeline_handle);
  vkCmdBindDescriptorSets(g_renderer.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_voxel_selector_pipeline.pipeline_layout, 0, 1, &s_voxel_selector_pipeline.descriptor_set[0], 0, 0);
  vkCmdDispatch(g_renderer.command_buffer, 1, 1, 1);
}
void svdb_place_voxel(void) {
  VkImageMemoryBarrier voxel_image_memory_barrier = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
    .newLayout = VK_IMAGE_LAYOUT_GENERAL,
    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .image = s_chunk_voxel_image.image_handle,
    .subresourceRange = {
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .baseMipLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1,
    },
    .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
    .dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
  };

  vkCmdBindPipeline(g_renderer.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_voxel_placer_pipeline.pipeline_handle);
  vkCmdBindDescriptorSets(g_renderer.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_voxel_placer_pipeline.pipeline_layout, 0, 1, &s_voxel_placer_pipeline.descriptor_set[0], 0, 0);
  vkCmdDispatch(g_renderer.command_buffer, 1, 1, 1);
  vkCmdPipelineBarrier(g_renderer.command_buffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, 0, 0, 0, 1, &voxel_image_memory_barrier);
}

void svdb_generate_world(VkCommandBuffer command_buffer, ivector3_t chunk_position, uint32_t chunk_index) {
  VkImageMemoryBarrier voxel_image_memory_barrier = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
    .newLayout = VK_IMAGE_LAYOUT_GENERAL,
    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .image = s_chunk_voxel_image.image_handle,
    .subresourceRange = {
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .baseMipLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1,
    },
    .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
    .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
  };

  svdb_world_generator_push_constant_t push_constant = {
    .chunk_position = chunk_position,
    .chunk_index = chunk_index,
  };

  vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_world_generator_pipeline.pipeline_handle);
  vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_world_generator_pipeline.pipeline_layout, 0, 1, &s_world_generator_pipeline.descriptor_set[0], 0, 0);

  push_constant.stage = 0;

  vkCmdPushConstants(command_buffer, s_world_generator_pipeline.pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(push_constant), &push_constant);
  vkCmdDispatch(command_buffer, 5, 5, 5);
  vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, 0, 0, 0, 1, &voxel_image_memory_barrier);

  push_constant.stage = 1;

  vkCmdPushConstants(command_buffer, s_world_generator_pipeline.pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(push_constant), &push_constant);
  vkCmdDispatch(command_buffer, 5, 5, 5);
  vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, 0, 0, 0, 1, &voxel_image_memory_barrier);

  push_constant.stage = 2;

  vkCmdPushConstants(command_buffer, s_world_generator_pipeline.pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(push_constant), &push_constant);
  vkCmdDispatch(command_buffer, 5, 5, 5);
  vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, 0, 0, 0, 1, &voxel_image_memory_barrier);
}
void svdb_generate_mask(VkCommandBuffer command_buffer, ivector3_t chunk_position, uint32_t chunk_index) {
  VkBufferMemoryBarrier mask_buffer_memory_barrier = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
    .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
    .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .buffer = s_chunk_mask_buffer.buffer_handle,
    .offset = 0,
    .size = VK_WHOLE_SIZE,
  };

  svdb_mask_generator_push_constant_t push_constant = {
    .chunk_position = chunk_position,
    .chunk_index = chunk_index,
  };

  vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_mask_generator_pipeline.pipeline_handle);
  vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_mask_generator_pipeline.pipeline_layout, 0, 1, &s_mask_generator_pipeline.descriptor_set[0], 0, 0);
  vkCmdPushConstants(command_buffer, s_mask_generator_pipeline.pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(push_constant), &push_constant);
  vkCmdDispatch(command_buffer, SVDB_CHUNK_SIZE, SVDB_CHUNK_SIZE, 6);
  vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, 0, 1, &mask_buffer_memory_barrier, 0, 0);
}
void svdb_generate_mesh(VkCommandBuffer command_buffer, ivector3_t chunk_position, uint32_t chunk_index) {
  VkBufferMemoryBarrier buffer_memory_barrier[] = {
    {
      .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .buffer = s_chunk_vertex_buffer.buffer_handle,
      .offset = 0,
      .size = VK_WHOLE_SIZE,
    },
    {
      .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .buffer = s_chunk_index_buffer.buffer_handle,
      .offset = 0,
      .size = VK_WHOLE_SIZE,
    },
    {
      .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .buffer = s_mesh_count_buffer.buffer_handle,
      .offset = 0,
      .size = VK_WHOLE_SIZE,
    },
  };

  svdb_mesh_generator_push_constant_t push_constant = {
    .chunk_position = chunk_position,
    .chunk_index = chunk_index,
  };

  vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_mesh_generator_pipeline.pipeline_handle);
  vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_mesh_generator_pipeline.pipeline_layout, 0, 1, &s_mesh_generator_pipeline.descriptor_set[0], 0, 0);
  vkCmdPushConstants(command_buffer, s_mesh_generator_pipeline.pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(push_constant), &push_constant);
  vkCmdDispatch(command_buffer, SVDB_CHUNK_SIZE, 1, 6);
  vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, 0, ARRAY_COUNT(buffer_memory_barrier), buffer_memory_barrier, 0, 0);
}
void svdb_generate_idraw(VkCommandBuffer command_buffer) {
  VkBufferMemoryBarrier buffer_memory_barrier[] = {
    {
      .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .buffer = s_indirect_draw_buffer.buffer_handle,
      .offset = 0,
      .size = VK_WHOLE_SIZE,
    },
  };

  uint32_t group_count = MAKE_DISPATCH_GROUP_COUNT(g_svdb.chunk_count, 64);

  vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_idraw_generator_pipeline.pipeline_handle);
  vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_idraw_generator_pipeline.pipeline_layout, 0, 1, &s_idraw_generator_pipeline.descriptor_set[0], 0, 0);
  vkCmdDispatch(command_buffer, group_count, 1, 1);
  vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT, 0, 0, 0, ARRAY_COUNT(buffer_memory_barrier), buffer_memory_barrier, 0, 0);
}

void svdb_draw_mesh(VkCommandBuffer command_buffer) {
  VkBuffer buffer[] = {s_chunk_vertex_buffer.buffer_handle, s_chunk_instance_buffer.buffer_handle};
  VkDeviceSize offset[] = {0, 0};

  vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, s_renderer_pipeline.pipeline_handle);
  vkCmdBindVertexBuffers(command_buffer, 0, 2, buffer, offset);
  vkCmdBindIndexBuffer(command_buffer, s_chunk_index_buffer.buffer_handle, 0, VK_INDEX_TYPE_UINT32);
  vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, s_renderer_pipeline.pipeline_layout, 0, 1, &s_renderer_pipeline.descriptor_set[0], 0, 0);
  vkCmdDrawIndexedIndirect(command_buffer, s_indirect_draw_buffer.buffer_handle, 0, g_svdb.chunk_count, sizeof(VkDrawIndexedIndirectCommand)); // TODO revalidate draw_count here..
  // vkCmdDrawIndexedIndirectCount(); /// Use this for when frustum culling comes back online..
}

static void svdb_create_cluster_info_buffer(void) {
  svdb_cluster_info_t cluster_info = {
    .chunk_count = g_svdb.chunk_count,
  };

  s_cluster_info_buffer.host_data = &cluster_info;

  buffer_create(&s_cluster_info_buffer);

  s_cluster_info_descriptor_buffer_info.offset = 0;
  s_cluster_info_descriptor_buffer_info.buffer = s_cluster_info_buffer.buffer_handle;
  s_cluster_info_descriptor_buffer_info.range = VK_WHOLE_SIZE;
}
static void svdb_create_chunk_mask_buffer(void) {
  s_chunk_mask_buffer.size = sizeof(svdb_chunk_mask_t) * g_svdb.chunk_count;

  buffer_create(&s_chunk_mask_buffer);

  s_chunk_mask_descriptor_buffer_info.offset = 0;
  s_chunk_mask_descriptor_buffer_info.buffer = s_chunk_mask_buffer.buffer_handle;
  s_chunk_mask_descriptor_buffer_info.range = VK_WHOLE_SIZE;
}
static void svdb_create_chunk_instance_buffer(void) {
  s_chunk_instance_buffer.size = sizeof(svdb_chunk_instance_t) * g_svdb.chunk_count,

  buffer_create(&s_chunk_instance_buffer);

  s_chunk_instance_descriptor_buffer_info.offset = 0;
  s_chunk_instance_descriptor_buffer_info.buffer = s_chunk_instance_buffer.buffer_handle;
  s_chunk_instance_descriptor_buffer_info.range = VK_WHOLE_SIZE;
}
static void svdb_create_chunk_face_buffer(void) {
  s_chunk_face_buffer.size = sizeof(svdb_chunk_face_t) * WORST_CASE_GREEDY_MESH_FACE_COUNT,

  buffer_create(&s_chunk_face_buffer);

  s_chunk_face_descriptor_buffer_info.offset = 0;
  s_chunk_face_descriptor_buffer_info.buffer = s_chunk_face_buffer.buffer_handle;
  s_chunk_face_descriptor_buffer_info.range = VK_WHOLE_SIZE;
}
static void svdb_create_chunk_vertex_buffer(void) {
  s_chunk_vertex_buffer.size = sizeof(svdb_chunk_vertex_t) * WORST_CASE_GREEDY_MESH_VERTEX_COUNT,

  buffer_create(&s_chunk_vertex_buffer);

  s_chunk_vertex_descriptor_buffer_info.offset = 0;
  s_chunk_vertex_descriptor_buffer_info.buffer = s_chunk_vertex_buffer.buffer_handle;
  s_chunk_vertex_descriptor_buffer_info.range = VK_WHOLE_SIZE;
}
static void svdb_create_chunk_index_buffer(void) {
  s_chunk_index_buffer.size = sizeof(svdb_chunk_index_t) * WORST_CASE_GREEDY_MESH_INDEX_COUNT,

  buffer_create(&s_chunk_index_buffer);

  s_chunk_index_descriptor_buffer_info.offset = 0;
  s_chunk_index_descriptor_buffer_info.buffer = s_chunk_index_buffer.buffer_handle;
  s_chunk_index_descriptor_buffer_info.range = VK_WHOLE_SIZE;
}
static void svdb_create_mesh_count_buffer(void) {
  s_mesh_count_buffer.size = sizeof(svdb_mesh_count_t) * g_svdb.chunk_count;

  buffer_create(&s_mesh_count_buffer);

  s_mesh_count_descriptor_buffer_info.offset = 0;
  s_mesh_count_descriptor_buffer_info.buffer = s_mesh_count_buffer.buffer_handle;
  s_mesh_count_descriptor_buffer_info.range = VK_WHOLE_SIZE;
}
static void svdb_create_indirect_draw_buffer(void) {
  s_indirect_draw_buffer.size = sizeof(VkDrawIndexedIndirectCommand) * g_svdb.chunk_count;

  buffer_create(&s_indirect_draw_buffer);

  s_indirect_draw_descriptor_buffer_info.offset = 0;
  s_indirect_draw_descriptor_buffer_info.buffer = s_indirect_draw_buffer.buffer_handle;
  s_indirect_draw_descriptor_buffer_info.range = VK_WHOLE_SIZE;
}
static void svdb_create_select_result_buffer(void) {
  buffer_create(&s_select_result_buffer);

  buffer_map(&s_select_result_buffer);

  g_svdb.select_result = (svdb_select_result_t *)s_select_result_buffer.device_data;

  s_select_result_descriptor_buffer_info.offset = 0;
  s_select_result_descriptor_buffer_info.buffer = s_select_result_buffer.buffer_handle;
  s_select_result_descriptor_buffer_info.range = VK_WHOLE_SIZE;
}
static void svdb_create_place_info_buffer(void) {
  buffer_create(&s_place_info_buffer);

  buffer_map(&s_place_info_buffer);

  g_svdb.place_info = (svdb_place_info_t *)s_place_info_buffer.device_data;

  s_place_info_descriptor_buffer_info.offset = 0;
  s_place_info_descriptor_buffer_info.buffer = s_place_info_buffer.buffer_handle;
  s_place_info_descriptor_buffer_info.range = VK_WHOLE_SIZE;
}
static void svdb_create_place_result_buffer(void) {
  buffer_create(&s_place_result_buffer);

  buffer_map(&s_place_result_buffer);

  g_svdb.place_result = (svdb_place_result_t *)s_place_result_buffer.device_data;

  s_place_result_descriptor_buffer_info.offset = 0;
  s_place_result_descriptor_buffer_info.buffer = s_place_result_buffer.buffer_handle;
  s_place_result_descriptor_buffer_info.range = VK_WHOLE_SIZE;
}
static void svdb_create_block_buffer(void) {
  buffer_create(&s_block_buffer);

  s_block_descriptor_buffer_info.offset = 0;
  s_block_descriptor_buffer_info.buffer = s_block_buffer.buffer_handle;
  s_block_descriptor_buffer_info.range = VK_WHOLE_SIZE;
}

static void svdb_create_chunk_voxel_image(void) {
  image_create(&s_chunk_voxel_image);

  s_chunk_voxel_descriptor_image_info.sampler = s_chunk_voxel_image.sampler;
  s_chunk_voxel_descriptor_image_info.imageView = s_chunk_voxel_image.image_view;
  s_chunk_voxel_descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
}
static void svdb_create_block_atlas_image(void) {
  uint8_t *image = imgutil_load_image_from_file(0, 0, 0, ROOT_DIR "/asset/block_atlas.png");

  s_block_atlas_image.host_data = image;

  image_create(&s_block_atlas_image);

  s_block_atlas_image.host_data = 0;

  imgutil_free_image(image);

  s_block_atlas_descriptor_image_info.sampler = s_block_atlas_image.sampler;
  s_block_atlas_descriptor_image_info.imageView = s_block_atlas_image.image_view;
  s_block_atlas_descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
}

static void svdb_update_voxel_selector_descriptor_set(void) {
  VkWriteDescriptorSet write_descriptor_set[] = {
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_voxel_selector_pipeline.descriptor_set[0],
      .dstBinding = 0,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = 1,
      .pImageInfo = 0,
      .pBufferInfo = &g_renderer.camera_info_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_voxel_selector_pipeline.descriptor_set[0],
      .dstBinding = 1,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
      .descriptorCount = 1,
      .pImageInfo = &s_chunk_voxel_descriptor_image_info,
      .pBufferInfo = 0,
      .pTexelBufferView = 0,
    },
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_voxel_selector_pipeline.descriptor_set[0],
      .dstBinding = 2,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      .descriptorCount = 1,
      .pImageInfo = 0,
      .pBufferInfo = &s_select_result_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
  };

  vkUpdateDescriptorSets(g_window.device, ARRAY_COUNT(write_descriptor_set), write_descriptor_set, 0, 0);
}
static void svdb_update_voxel_placer_descriptor_set(void) {
  VkWriteDescriptorSet write_descriptor_set[] = {
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_voxel_placer_pipeline.descriptor_set[0],
      .dstBinding = 0,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = 1,
      .pImageInfo = 0,
      .pBufferInfo = &s_place_info_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_voxel_placer_pipeline.descriptor_set[0],
      .dstBinding = 1,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
      .descriptorCount = 1,
      .pImageInfo = &s_chunk_voxel_descriptor_image_info,
      .pBufferInfo = 0,
      .pTexelBufferView = 0,
    },
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_voxel_placer_pipeline.descriptor_set[0],
      .dstBinding = 2,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      .descriptorCount = 1,
      .pImageInfo = 0,
      .pBufferInfo = &s_place_result_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
  };

  vkUpdateDescriptorSets(g_window.device, ARRAY_COUNT(write_descriptor_set), write_descriptor_set, 0, 0);
}
static void svdb_update_world_generator_descriptor_set(void) {
  VkWriteDescriptorSet write_descriptor_set[] = {
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_world_generator_pipeline.descriptor_set[0],
      .dstBinding = 0,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      .descriptorCount = 1,
      .pImageInfo = 0,
      .pBufferInfo = &s_chunk_instance_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_world_generator_pipeline.descriptor_set[0],
      .dstBinding = 1,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
      .descriptorCount = 1,
      .pImageInfo = &s_chunk_voxel_descriptor_image_info,
      .pBufferInfo = 0,
      .pTexelBufferView = 0,
    },
  };

  vkUpdateDescriptorSets(g_window.device, ARRAY_COUNT(write_descriptor_set), write_descriptor_set, 0, 0);
}
static void svdb_update_mask_generator_descriptor_set(void) {
  VkWriteDescriptorSet write_descriptor_set[] = {
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_mask_generator_pipeline.descriptor_set[0],
      .dstBinding = 0,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
      .descriptorCount = 1,
      .pImageInfo = &s_chunk_voxel_descriptor_image_info,
      .pBufferInfo = 0,
      .pTexelBufferView = 0,
    },
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_mask_generator_pipeline.descriptor_set[0],
      .dstBinding = 1,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      .descriptorCount = 1,
      .pImageInfo = 0,
      .pBufferInfo = &s_chunk_mask_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
  };

  vkUpdateDescriptorSets(g_window.device, ARRAY_COUNT(write_descriptor_set), write_descriptor_set, 0, 0);
}
static void svdb_update_mesh_generator_descriptor_set(void) {
  VkWriteDescriptorSet write_descriptor_set[] = {
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_mesh_generator_pipeline.descriptor_set[0],
      .dstBinding = 0,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      .descriptorCount = 1,
      .pImageInfo = 0,
      .pBufferInfo = &s_chunk_mask_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_mesh_generator_pipeline.descriptor_set[0],
      .dstBinding = 1,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      .descriptorCount = 1,
      .pImageInfo = 0,
      .pBufferInfo = &s_chunk_face_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_mesh_generator_pipeline.descriptor_set[0],
      .dstBinding = 2,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      .descriptorCount = 1,
      .pImageInfo = 0,
      .pBufferInfo = &s_chunk_vertex_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_mesh_generator_pipeline.descriptor_set[0],
      .dstBinding = 3,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      .descriptorCount = 1,
      .pImageInfo = 0,
      .pBufferInfo = &s_chunk_index_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_mesh_generator_pipeline.descriptor_set[0],
      .dstBinding = 4,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      .descriptorCount = 1,
      .pImageInfo = 0,
      .pBufferInfo = &s_mesh_count_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_mesh_generator_pipeline.descriptor_set[0],
      .dstBinding = 5,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      .descriptorCount = 1,
      .pImageInfo = 0,
      .pBufferInfo = &s_block_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_mesh_generator_pipeline.descriptor_set[0],
      .dstBinding = 6,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
      .descriptorCount = 1,
      .pImageInfo = &s_chunk_voxel_descriptor_image_info,
      .pBufferInfo = 0,
      .pTexelBufferView = 0,
    },
  };

  vkUpdateDescriptorSets(g_window.device, ARRAY_COUNT(write_descriptor_set), write_descriptor_set, 0, 0);
}
static void svdb_update_idraw_generator_descriptor_set(void) {
  VkWriteDescriptorSet write_descriptor_set[] = {
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_idraw_generator_pipeline.descriptor_set[0],
      .dstBinding = 0,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = 1,
      .pImageInfo = 0,
      .pBufferInfo = &s_cluster_info_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_idraw_generator_pipeline.descriptor_set[0],
      .dstBinding = 1,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      .descriptorCount = 1,
      .pImageInfo = 0,
      .pBufferInfo = &s_mesh_count_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_idraw_generator_pipeline.descriptor_set[0],
      .dstBinding = 2,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      .descriptorCount = 1,
      .pImageInfo = 0,
      .pBufferInfo = &s_indirect_draw_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
  };

  vkUpdateDescriptorSets(g_window.device, ARRAY_COUNT(write_descriptor_set), write_descriptor_set, 0, 0);
}
static void svdb_update_renderer_descriptor_set(void) {
  VkWriteDescriptorSet write_descriptor_set[] = {
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_renderer_pipeline.descriptor_set[0],
      .dstBinding = 0,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = 1,
      .pImageInfo = 0,
      .pBufferInfo = &g_renderer.camera_info_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_renderer_pipeline.descriptor_set[0],
      .dstBinding = 1,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      .descriptorCount = 1,
      .pImageInfo = 0,
      .pBufferInfo = &s_chunk_face_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_renderer_pipeline.descriptor_set[0],
      .dstBinding = 2,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .descriptorCount = 1,
      .pImageInfo = &s_block_atlas_descriptor_image_info,
      .pBufferInfo = 0,
      .pTexelBufferView = 0,
    },
  };

  vkUpdateDescriptorSets(g_window.device, ARRAY_COUNT(write_descriptor_set), write_descriptor_set, 0, 0);
}

static void svdb_destroy_buffer(void) {
  buffer_destroy(&s_cluster_info_buffer);
  buffer_destroy(&s_chunk_instance_buffer);
  buffer_destroy(&s_chunk_face_buffer);
  buffer_destroy(&s_chunk_vertex_buffer);
  buffer_destroy(&s_chunk_index_buffer);
  buffer_destroy(&s_chunk_mask_buffer);
  buffer_destroy(&s_mesh_count_buffer);
  buffer_destroy(&s_indirect_draw_buffer);
  buffer_destroy(&s_select_result_buffer);
  buffer_destroy(&s_place_info_buffer);
  buffer_destroy(&s_place_result_buffer);
  buffer_destroy(&s_block_buffer);
}
static void svdb_destroy_image(void) {
  image_destroy(&s_chunk_voxel_image);
  image_destroy(&s_block_atlas_image);
}
