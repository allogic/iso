#include <pch.h>

//    32 ^ 3 sides        = 32768
// 32768 / 2 checkerboard = 16384
// 16384 x 6 faces        = 98304
// 98304 x 4 vertices     = 393216
// 98304 x 6 indices      = 589824
#define WORST_CASE_GREEDY_MESH_VERTEX_COUNT (400000)
#define WORST_CASE_GREEDY_MESH_INDEX_COUNT (600000)

typedef buffer_t double_buffer_t[2];
typedef VkDescriptorBufferInfo VkDoubleDescriptorBufferInfo[2];

static void svdb_create_chunk_info_buffer(void);
static void svdb_create_chunk_mask_buffer(void);
static void svdb_create_chunk_vertex_buffer(void);
static void svdb_create_chunk_index_buffer(void);
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
static void svdb_update_renderer_descriptor_set(void);

static void svdb_destroy_buffer(void);
static void svdb_destroy_image(void);

static uint32_t s_active_buffer = 0;

static VkVertexInputBindingDescription const s_chunk_vertex_input_binding_description[] = {
  {
    .binding = 0,
    .stride = sizeof(svdb_chunk_vertex_t),
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
  },
};

static VkVertexInputAttributeDescription const s_chunk_vertex_input_attribute_description[] = {
  {
    .location = 0,
    .binding = 0,
    .format = VK_FORMAT_R32G32B32A32_SFLOAT,
    .offset = 0,
  },
  {
    .location = 1,
    .binding = 0,
    .format = VK_FORMAT_R32G32B32A32_SFLOAT,
    .offset = OFFSET_OF(svdb_chunk_vertex_t, color),
  },
  {
    .location = 2,
    .binding = 0,
    .format = VK_FORMAT_R32G32B32A32_SFLOAT,
    .offset = OFFSET_OF(svdb_chunk_vertex_t, uv),
  },
  {
    .location = 3,
    .binding = 0,
    .format = VK_FORMAT_R32_UINT,
    .offset = OFFSET_OF(svdb_chunk_vertex_t, atlas_id),
  },
};

static VkDescriptorPoolSize const s_voxel_selector_descriptor_pool_size[] = {
  {
    .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
  },
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    .descriptorCount = SVDB_CHUNK_COUNT,
  },
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 1,
  },
};
static VkDescriptorPoolSize const s_voxel_placer_descriptor_pool_size[] = {
  {
    .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
  },
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    .descriptorCount = SVDB_CHUNK_COUNT,
  },
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 1,
  },
};
static VkDescriptorPoolSize const s_world_generator_descriptor_pool_size[] = {
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    .descriptorCount = SVDB_CHUNK_COUNT,
  },
};
static VkDescriptorPoolSize const s_mask_generator_descriptor_pool_size[] = {
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    .descriptorCount = SVDB_CHUNK_COUNT,
  },
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 1,
  },
};
static VkDescriptorPoolSize const s_mesh_generator_descriptor_pool_size[] = {
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 5 * 2,
  },
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    .descriptorCount = 1 * 2,
  },
};
static VkDescriptorPoolSize const s_renderer_descriptor_pool_size[] = {
  {
    .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
  },
  {
    .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    .descriptorCount = 1,
  },
};

static VkDescriptorSetLayoutBinding const s_voxel_selector_descriptor_set_layout_binding[] = {
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
    .descriptorCount = SVDB_CHUNK_COUNT,
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
static VkDescriptorSetLayoutBinding const s_voxel_placer_descriptor_set_layout_binding[] = {
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
    .descriptorCount = SVDB_CHUNK_COUNT,
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
static VkDescriptorSetLayoutBinding const s_world_generator_descriptor_set_layout_binding[] = {
  {
    .binding = 0,
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    .descriptorCount = SVDB_CHUNK_COUNT,
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .pImmutableSamplers = 0,
  },
};
static VkDescriptorSetLayoutBinding const s_mask_generator_descriptor_set_layout_binding[] = {
  {
    .binding = 0,
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    .descriptorCount = SVDB_CHUNK_COUNT,
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
static VkDescriptorSetLayoutBinding const s_mesh_generator_descriptor_set_layout_binding[] = {
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
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .pImmutableSamplers = 0,
  },
};
static VkDescriptorSetLayoutBinding const s_renderer_descriptor_set_layout_binding[] = {
  {
    .binding = 0,
    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    .pImmutableSamplers = 0,
  },
  {
    .binding = 1,
    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
    .pImmutableSamplers = 0,
  },
};

static VkPushConstantRange const s_world_generator_push_constant_range[] = {
  {
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .offset = 0,
    .size = sizeof(svdb_world_generator_push_constant_t),
  },
};
static VkPushConstantRange const s_mask_generator_push_constant_range[] = {
  {
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .offset = 0,
    .size = sizeof(svdb_mask_generator_push_constant_t),
  },
};
static VkPushConstantRange const s_mesh_generator_push_constant_range[] = {
  {
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .offset = 0,
    .size = sizeof(svdb_mesh_generator_push_constant_t),
  },
};
static VkPushConstantRange const s_renderer_push_constant_range[] = {
  {
    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    .offset = 0,
    .size = sizeof(svdb_renderer_push_constant_t),
  },
};

static double_buffer_t s_chunk_info_buffer = {
  {
    .size = sizeof(svdb_chunk_info_t) * SVDB_CHUNK_COUNT,
    .buffer_usage_flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
    .memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
  },
  {
    .size = sizeof(svdb_chunk_info_t) * SVDB_CHUNK_COUNT,
    .buffer_usage_flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
    .memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
  },
};
static double_buffer_t *s_chunk_vertex_buffer = 0;
static double_buffer_t *s_chunk_index_buffer = 0;
static buffer_t s_chunk_mask_buffer = {
  .size = sizeof(svdb_chunk_mask_t) * SVDB_CHUNK_COUNT,
  .buffer_usage_flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
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

static image_t *s_chunk_voxel_image = 0;
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

static VkDoubleDescriptorBufferInfo s_chunk_info_descriptor_buffer_info = {0};
static VkDescriptorBufferInfo s_chunk_mask_descriptor_buffer_info = {0};
static VkDoubleDescriptorBufferInfo *s_chunk_vertex_descriptor_buffer_info = 0;
static VkDoubleDescriptorBufferInfo *s_chunk_index_descriptor_buffer_info = 0;
static VkDescriptorBufferInfo s_select_result_descriptor_buffer_info = {0};
static VkDescriptorBufferInfo s_place_info_descriptor_buffer_info = {0};
static VkDescriptorBufferInfo s_place_result_descriptor_buffer_info = {0};
static VkDescriptorBufferInfo s_block_descriptor_buffer_info = {0};

static VkDescriptorImageInfo *s_chunk_voxel_descriptor_image_info = 0;
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
  .descriptor_set_count = SVDB_CHUNK_COUNT * 2,
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
  .vertex_input_binding_description = s_chunk_vertex_input_binding_description,
  .vertex_input_binding_description_count = ARRAY_COUNT(s_chunk_vertex_input_binding_description),
  .vertex_input_attribute_description = s_chunk_vertex_input_attribute_description,
  .vertex_input_attribute_description_count = ARRAY_COUNT(s_chunk_vertex_input_attribute_description),
  .push_constant_range = s_renderer_push_constant_range,
  .push_constant_range_count = ARRAY_COUNT(s_renderer_push_constant_range),
  .descriptor_pool_size = s_renderer_descriptor_pool_size,
  .descriptor_pool_size_count = ARRAY_COUNT(s_renderer_descriptor_pool_size),
  .descriptor_set_layout_binding = s_renderer_descriptor_set_layout_binding,
  .descriptor_set_layout_binding_count = ARRAY_COUNT(s_renderer_descriptor_set_layout_binding),
  .descriptor_set_count = SVDB_CHUNK_COUNT,
  .render_pass = &g_renderpass,
};

svdb_t g_svdb = {0};

void svdb_create(void) {
  g_svdb.generate_world = 1;

  svdb_create_chunk_info_buffer();
  svdb_create_chunk_mask_buffer();
  svdb_create_chunk_vertex_buffer();
  svdb_create_chunk_index_buffer();
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
  pipeline_create(&s_renderer_pipeline);

  svdb_update_descriptors();
}
void svdb_update_descriptors(void) {
  svdb_update_voxel_selector_descriptor_set();
  svdb_update_voxel_placer_descriptor_set();
  svdb_update_world_generator_descriptor_set();
  svdb_update_mask_generator_descriptor_set();
  svdb_update_mesh_generator_descriptor_set();
  svdb_update_renderer_descriptor_set();
}
void svdb_debug(void) {
  uint32_t chunk_index = 0;
  uint32_t chunk_count = SVDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    ivector3_t chunk_position = svdb_chunk_index_to_position(chunk_index);

    renderer_draw_debug_box(
      (vector3_t){(float)chunk_position.x * SVDB_CHUNK_SIZE, (float)chunk_position.y * SVDB_CHUNK_SIZE, (float)chunk_position.z * SVDB_CHUNK_SIZE},
      (vector3_t){(float)SVDB_CHUNK_SIZE, (float)SVDB_CHUNK_SIZE, (float)SVDB_CHUNK_SIZE},
      (vector4_t){1.0F, 1.0F, 1.0F, 1.0F});

    chunk_index++;
  }
}
void svdb_draw(void) {
  uint32_t front_buffer = s_active_buffer;

  svdb_chunk_info_t *chunk_info = (svdb_chunk_info_t *)s_chunk_info_buffer[front_buffer].device_data;

  vkCmdBindPipeline(g_window.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, s_renderer_pipeline.pipeline_handle);

  svdb_renderer_push_constant_t svdb_renderer_push_constant = {0};

  uint32_t chunk_index = 0;
  uint32_t chunk_count = SVDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    svdb_renderer_push_constant.chunk_position = svdb_chunk_index_to_position(chunk_index);
    svdb_renderer_push_constant.chunk_index = chunk_index;

    VkDeviceSize vertex_offset[] = {0};

    vkCmdBindVertexBuffers(g_window.command_buffer, 0, 1, &s_chunk_vertex_buffer[chunk_index][front_buffer].buffer_handle, vertex_offset);
    vkCmdBindIndexBuffer(g_window.command_buffer, s_chunk_index_buffer[chunk_index][front_buffer].buffer_handle, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(g_window.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, s_renderer_pipeline.pipeline_layout, 0, 1, &s_renderer_pipeline.descriptor_set[chunk_index], 0, 0);
    vkCmdPushConstants(g_window.command_buffer, s_renderer_pipeline.pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(svdb_renderer_push_constant), &svdb_renderer_push_constant);
    vkCmdDrawIndexed(g_window.command_buffer, chunk_info[chunk_index].index_count, 1, 0, 0, 0);

    chunk_index++;
  }
}
void svdb_destroy(void) {
  pipeline_destroy(&s_voxel_selector_pipeline);
  pipeline_destroy(&s_voxel_placer_pipeline);
  pipeline_destroy(&s_world_generator_pipeline);
  pipeline_destroy(&s_mask_generator_pipeline);
  pipeline_destroy(&s_mesh_generator_pipeline);
  pipeline_destroy(&s_renderer_pipeline);

  svdb_destroy_buffer();
  svdb_destroy_image();
}

void svdb_select_voxel(void) {
  vkCmdBindPipeline(g_window.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_voxel_selector_pipeline.pipeline_handle);
  vkCmdBindDescriptorSets(g_window.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_voxel_selector_pipeline.pipeline_layout, 0, 1, &s_voxel_selector_pipeline.descriptor_set[0], 0, 0);
  vkCmdDispatch(g_window.command_buffer, 1, 1, 1);
}
void svdb_place_voxel(void) {
  vkCmdBindPipeline(g_window.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_voxel_placer_pipeline.pipeline_handle);
  vkCmdBindDescriptorSets(g_window.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_voxel_placer_pipeline.pipeline_layout, 0, 1, &s_voxel_placer_pipeline.descriptor_set[0], 0, 0);
  vkCmdDispatch(g_window.command_buffer, 1, 1, 1);

  VkImageMemoryBarrier image_memory_barrier = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
    .newLayout = VK_IMAGE_LAYOUT_GENERAL,
    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .image = s_chunk_voxel_image[0].image_handle, // TODO: do this for current and all surrounding chunks..
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

  vkCmdPipelineBarrier(
    g_window.command_buffer,
    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
    0,
    0,
    0,
    0,
    0,
    1,
    &image_memory_barrier);
}

void svdb_generate_world(void) {
  vkCmdBindPipeline(g_window.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_world_generator_pipeline.pipeline_handle);
  vkCmdBindDescriptorSets(g_window.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_world_generator_pipeline.pipeline_layout, 0, 1, &s_world_generator_pipeline.descriptor_set[0], 0, 0);

  svdb_world_generator_push_constant_t svdb_world_generator_push_constant = {0};

  uint32_t chunk_index = 0;
  uint32_t chunk_count = SVDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    svdb_world_generator_push_constant.chunk_position = svdb_chunk_index_to_position(chunk_index);
    svdb_world_generator_push_constant.chunk_index = chunk_index;
    svdb_world_generator_push_constant.stage = 0;

    vkCmdPushConstants(g_window.command_buffer, s_world_generator_pipeline.pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(svdb_world_generator_push_constant), &svdb_world_generator_push_constant);
    vkCmdDispatch(g_window.command_buffer, 4, 4, 4);

    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
      .newLayout = VK_IMAGE_LAYOUT_GENERAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = s_chunk_voxel_image[chunk_index].image_handle,
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

    vkCmdPipelineBarrier(
      g_window.command_buffer,
      VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
      VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
      0,
      0,
      0,
      0,
      0,
      1,
      &image_memory_barrier);

    chunk_index++;
  }

  chunk_index = 0;

  while (chunk_index < chunk_count) {

    svdb_world_generator_push_constant.chunk_position = svdb_chunk_index_to_position(chunk_index);
    svdb_world_generator_push_constant.chunk_index = chunk_index;
    svdb_world_generator_push_constant.stage = 1;

    vkCmdPushConstants(g_window.command_buffer, s_world_generator_pipeline.pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(svdb_world_generator_push_constant), &svdb_world_generator_push_constant);
    vkCmdDispatch(g_window.command_buffer, 4, 4, 4);

    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
      .newLayout = VK_IMAGE_LAYOUT_GENERAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = s_chunk_voxel_image[chunk_index].image_handle,
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

    vkCmdPipelineBarrier(
      g_window.command_buffer,
      VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
      VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
      0,
      0,
      0,
      0,
      0,
      1,
      &image_memory_barrier);

    chunk_index++;
  }
}
void svdb_generate_mask(void) {
  vkCmdBindPipeline(g_window.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_mask_generator_pipeline.pipeline_handle);
  vkCmdBindDescriptorSets(g_window.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_mask_generator_pipeline.pipeline_layout, 0, 1, &s_mask_generator_pipeline.descriptor_set[0], 0, 0);

  svdb_mask_generator_push_constant_t svdb_mask_generator_push_constant = {0};

  uint32_t chunk_index = 0;
  uint32_t chunk_count = SVDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    svdb_mask_generator_push_constant.chunk_position = svdb_chunk_index_to_position(chunk_index);
    svdb_mask_generator_push_constant.chunk_index = chunk_index;

    vkCmdPushConstants(g_window.command_buffer, s_mask_generator_pipeline.pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(svdb_mask_generator_push_constant), &svdb_mask_generator_push_constant);
    vkCmdDispatch(g_window.command_buffer, SVDB_CHUNK_SIZE, SVDB_CHUNK_SIZE, 6);

    chunk_index++;
  }

  VkBufferMemoryBarrier buffer_memory_barrier = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
    .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
    .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .buffer = s_chunk_mask_buffer.buffer_handle,
    .offset = 0,
    .size = VK_WHOLE_SIZE,
  };

  vkCmdPipelineBarrier(
    g_window.command_buffer,
    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
    0,
    0,
    0,
    1,
    &buffer_memory_barrier,
    0,
    0);
}
void svdb_generate_mesh(void) {
  uint32_t back_buffer = !s_active_buffer;

  svdb_chunk_info_t *chunk_info = (svdb_chunk_info_t *)s_chunk_info_buffer[back_buffer].device_data;

  vkCmdBindPipeline(g_window.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_mesh_generator_pipeline.pipeline_handle);

  svdb_mesh_generator_push_constant_t svdb_mesh_generator_push_constant = {0};

  uint32_t chunk_index = 0;
  uint32_t chunk_count = SVDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    uint32_t chunk_descriptor_index = chunk_index + (back_buffer * SVDB_CHUNK_COUNT);

    chunk_info[chunk_index].vertex_count = 0;
    chunk_info[chunk_index].index_count = 0;

    svdb_mesh_generator_push_constant.chunk_position = svdb_chunk_index_to_position(chunk_index);
    svdb_mesh_generator_push_constant.chunk_index = chunk_index;

    vkCmdBindDescriptorSets(g_window.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_mesh_generator_pipeline.pipeline_layout, 0, 1, &s_mesh_generator_pipeline.descriptor_set[chunk_descriptor_index], 0, 0);
    vkCmdPushConstants(g_window.command_buffer, s_mesh_generator_pipeline.pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(svdb_mesh_generator_push_constant), &svdb_mesh_generator_push_constant);
    vkCmdDispatch(g_window.command_buffer, SVDB_CHUNK_SIZE, 1, 6);

    VkBufferMemoryBarrier buffer_memory_barrier[] = {
      {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .buffer = s_chunk_vertex_buffer[chunk_index][back_buffer].buffer_handle,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
      },
      {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_INDEX_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .buffer = s_chunk_index_buffer[chunk_index][back_buffer].buffer_handle,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
      },
      {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .buffer = s_chunk_info_buffer[back_buffer].buffer_handle,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
      },
    };

    vkCmdPipelineBarrier(
      g_window.command_buffer,
      VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
      VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
      0,
      0,
      0,
      ARRAY_COUNT(buffer_memory_barrier),
      buffer_memory_barrier,
      0,
      0);

    chunk_index++;
  }
}

void svdb_swap_buffer(void) {
  if (g_svdb.is_dirty) {

    g_svdb.is_dirty = 0;

    s_active_buffer = !s_active_buffer;
  }
}

uint32_t svdb_chunk_position_to_index(ivector3_t chunk_position) {
  return (chunk_position.x) +
         (chunk_position.y * SVDB_DIM_X) +
         (chunk_position.z * SVDB_DIM_X * SVDB_DIM_Y);
}
ivector3_t svdb_chunk_index_to_position(uint32_t chunk_index) {
  return (ivector3_t){
    chunk_index % SVDB_DIM_X,
    (chunk_index / SVDB_DIM_X) % SVDB_DIM_Y,
    chunk_index / (SVDB_DIM_X * SVDB_DIM_Y),
  };
}

static void svdb_create_chunk_info_buffer(void) {
  buffer_create(&s_chunk_info_buffer[0]);
  buffer_create(&s_chunk_info_buffer[1]);

  buffer_map(&s_chunk_info_buffer[0]);
  buffer_map(&s_chunk_info_buffer[1]);

  s_chunk_info_descriptor_buffer_info[0].offset = 0;
  s_chunk_info_descriptor_buffer_info[0].buffer = s_chunk_info_buffer[0].buffer_handle;
  s_chunk_info_descriptor_buffer_info[0].range = VK_WHOLE_SIZE;

  s_chunk_info_descriptor_buffer_info[1].offset = 0;
  s_chunk_info_descriptor_buffer_info[1].buffer = s_chunk_info_buffer[1].buffer_handle;
  s_chunk_info_descriptor_buffer_info[1].range = VK_WHOLE_SIZE;
}
static void svdb_create_chunk_mask_buffer(void) {
  buffer_create(&s_chunk_mask_buffer);

  s_chunk_mask_descriptor_buffer_info.offset = 0;
  s_chunk_mask_descriptor_buffer_info.buffer = s_chunk_mask_buffer.buffer_handle;
  s_chunk_mask_descriptor_buffer_info.range = VK_WHOLE_SIZE;
}
static void svdb_create_chunk_vertex_buffer(void) {
  s_chunk_vertex_buffer = (double_buffer_t *)HEAP_ALLOC(sizeof(double_buffer_t) * SVDB_CHUNK_COUNT, 1, 0);
  s_chunk_vertex_descriptor_buffer_info = (VkDoubleDescriptorBufferInfo *)HEAP_ALLOC(sizeof(VkDoubleDescriptorBufferInfo) * SVDB_CHUNK_COUNT, 1, 0);

  uint32_t chunk_index = 0;
  uint32_t chunk_count = SVDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    s_chunk_vertex_buffer[chunk_index][0].size = sizeof(svdb_chunk_vertex_t) * WORST_CASE_GREEDY_MESH_VERTEX_COUNT;
    s_chunk_vertex_buffer[chunk_index][0].buffer_usage_flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    s_chunk_vertex_buffer[chunk_index][0].memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    s_chunk_vertex_buffer[chunk_index][1].size = sizeof(svdb_chunk_vertex_t) * WORST_CASE_GREEDY_MESH_VERTEX_COUNT;
    s_chunk_vertex_buffer[chunk_index][1].buffer_usage_flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    s_chunk_vertex_buffer[chunk_index][1].memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    buffer_create(&s_chunk_vertex_buffer[chunk_index][0]);
    buffer_create(&s_chunk_vertex_buffer[chunk_index][1]);

    s_chunk_vertex_descriptor_buffer_info[chunk_index][0].offset = 0;
    s_chunk_vertex_descriptor_buffer_info[chunk_index][0].buffer = s_chunk_vertex_buffer[chunk_index][0].buffer_handle;
    s_chunk_vertex_descriptor_buffer_info[chunk_index][0].range = VK_WHOLE_SIZE;

    s_chunk_vertex_descriptor_buffer_info[chunk_index][1].offset = 0;
    s_chunk_vertex_descriptor_buffer_info[chunk_index][1].buffer = s_chunk_vertex_buffer[chunk_index][1].buffer_handle;
    s_chunk_vertex_descriptor_buffer_info[chunk_index][1].range = VK_WHOLE_SIZE;

    chunk_index++;
  }
}
static void svdb_create_chunk_index_buffer(void) {
  s_chunk_index_buffer = (double_buffer_t *)HEAP_ALLOC(sizeof(double_buffer_t) * SVDB_CHUNK_COUNT, 1, 0);
  s_chunk_index_descriptor_buffer_info = (VkDoubleDescriptorBufferInfo *)HEAP_ALLOC(sizeof(VkDoubleDescriptorBufferInfo) * SVDB_CHUNK_COUNT, 1, 0);

  uint32_t chunk_index = 0;
  uint32_t chunk_count = SVDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    s_chunk_index_buffer[chunk_index][0].size = sizeof(svdb_chunk_index_t) * WORST_CASE_GREEDY_MESH_INDEX_COUNT;
    s_chunk_index_buffer[chunk_index][0].buffer_usage_flags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    s_chunk_index_buffer[chunk_index][0].memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    s_chunk_index_buffer[chunk_index][1].size = sizeof(svdb_chunk_index_t) * WORST_CASE_GREEDY_MESH_INDEX_COUNT;
    s_chunk_index_buffer[chunk_index][1].buffer_usage_flags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    s_chunk_index_buffer[chunk_index][1].memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    buffer_create(&s_chunk_index_buffer[chunk_index][0]);
    buffer_create(&s_chunk_index_buffer[chunk_index][1]);

    s_chunk_index_descriptor_buffer_info[chunk_index][0].offset = 0;
    s_chunk_index_descriptor_buffer_info[chunk_index][0].buffer = s_chunk_index_buffer[chunk_index][0].buffer_handle;
    s_chunk_index_descriptor_buffer_info[chunk_index][0].range = VK_WHOLE_SIZE;

    s_chunk_index_descriptor_buffer_info[chunk_index][1].offset = 0;
    s_chunk_index_descriptor_buffer_info[chunk_index][1].buffer = s_chunk_index_buffer[chunk_index][1].buffer_handle;
    s_chunk_index_descriptor_buffer_info[chunk_index][1].range = VK_WHOLE_SIZE;

    chunk_index++;
  }
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
  s_chunk_voxel_image = (image_t *)HEAP_ALLOC(sizeof(image_t) * SVDB_CHUNK_COUNT, 1, 0);
  s_chunk_voxel_descriptor_image_info = (VkDescriptorImageInfo *)HEAP_ALLOC(sizeof(VkDescriptorImageInfo) * SVDB_CHUNK_COUNT, 1, 0);

  uint32_t chunk_index = 0;
  uint32_t chunk_count = SVDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    s_chunk_voxel_image[chunk_index].width = SVDB_CHUNK_SIZE,
    s_chunk_voxel_image[chunk_index].height = SVDB_CHUNK_SIZE,
    s_chunk_voxel_image[chunk_index].depth = SVDB_CHUNK_SIZE,
    s_chunk_voxel_image[chunk_index].channel = 1,
    s_chunk_voxel_image[chunk_index].element_size = sizeof(uint32_t),
    s_chunk_voxel_image[chunk_index].format = VK_FORMAT_R32_UINT,
    s_chunk_voxel_image[chunk_index].filter = VK_FILTER_NEAREST,
    s_chunk_voxel_image[chunk_index].image_usage_flags = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
    s_chunk_voxel_image[chunk_index].image_type = VK_IMAGE_TYPE_3D,
    s_chunk_voxel_image[chunk_index].image_view_type = VK_IMAGE_VIEW_TYPE_3D,
    s_chunk_voxel_image[chunk_index].image_aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT,
    s_chunk_voxel_image[chunk_index].image_tiling = VK_IMAGE_TILING_OPTIMAL,

    image_create(&s_chunk_voxel_image[chunk_index]);

    s_chunk_voxel_descriptor_image_info[chunk_index].sampler = s_chunk_voxel_image[chunk_index].sampler;
    s_chunk_voxel_descriptor_image_info[chunk_index].imageView = s_chunk_voxel_image[chunk_index].image_view;
    s_chunk_voxel_descriptor_image_info[chunk_index].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    chunk_index++;
  }
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
      .descriptorCount = SVDB_CHUNK_COUNT,
      .pImageInfo = s_chunk_voxel_descriptor_image_info,
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
      .descriptorCount = SVDB_CHUNK_COUNT,
      .pImageInfo = s_chunk_voxel_descriptor_image_info,
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
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
      .descriptorCount = SVDB_CHUNK_COUNT,
      .pImageInfo = s_chunk_voxel_descriptor_image_info,
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
      .descriptorCount = SVDB_CHUNK_COUNT,
      .pImageInfo = s_chunk_voxel_descriptor_image_info,
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
  uint32_t buffer_index = 0;
  uint32_t buffer_count = 2;

  while (buffer_index < buffer_count) {

    uint32_t chunk_index = 0;
    uint32_t chunk_count = SVDB_CHUNK_COUNT;

    while (chunk_index < chunk_count) {

      VkWriteDescriptorSet write_descriptor_set[] = {
        {
          .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
          .pNext = 0,
          .dstSet = s_mesh_generator_pipeline.descriptor_set[chunk_index * 2 + buffer_index],
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
          .dstSet = s_mesh_generator_pipeline.descriptor_set[chunk_index * 2 + buffer_index],
          .dstBinding = 1,
          .dstArrayElement = 0,
          .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
          .descriptorCount = 1,
          .pImageInfo = 0,
          .pBufferInfo = &s_chunk_vertex_descriptor_buffer_info[chunk_index][buffer_index],
          .pTexelBufferView = 0,
        },
        {
          .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
          .pNext = 0,
          .dstSet = s_mesh_generator_pipeline.descriptor_set[chunk_index * 2 + buffer_index],
          .dstBinding = 2,
          .dstArrayElement = 0,
          .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
          .descriptorCount = 1,
          .pImageInfo = 0,
          .pBufferInfo = &s_chunk_index_descriptor_buffer_info[chunk_index][buffer_index],
          .pTexelBufferView = 0,
        },
        {
          .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
          .pNext = 0,
          .dstSet = s_mesh_generator_pipeline.descriptor_set[chunk_index * 2 + buffer_index],
          .dstBinding = 3,
          .dstArrayElement = 0,
          .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
          .descriptorCount = 1,
          .pImageInfo = 0,
          .pBufferInfo = &s_chunk_info_descriptor_buffer_info[buffer_index],
          .pTexelBufferView = 0,
        },
        {
          .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
          .pNext = 0,
          .dstSet = s_mesh_generator_pipeline.descriptor_set[chunk_index * 2 + buffer_index],
          .dstBinding = 4,
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
          .dstSet = s_mesh_generator_pipeline.descriptor_set[chunk_index * 2 + buffer_index],
          .dstBinding = 5,
          .dstArrayElement = 0,
          .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
          .descriptorCount = 1,
          .pImageInfo = &s_chunk_voxel_descriptor_image_info[chunk_index],
          .pBufferInfo = 0,
          .pTexelBufferView = 0,
        },
      };

      vkUpdateDescriptorSets(g_window.device, ARRAY_COUNT(write_descriptor_set), write_descriptor_set, 0, 0);

      chunk_index++;
    }

    buffer_index++;
  }
}
static void svdb_update_renderer_descriptor_set(void) {
  uint32_t chunk_index = 0;
  uint32_t chunk_count = SVDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    VkWriteDescriptorSet write_descriptor_set[] = {
      {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = 0,
        .dstSet = s_renderer_pipeline.descriptor_set[chunk_index],
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
        .dstSet = s_renderer_pipeline.descriptor_set[chunk_index],
        .dstBinding = 1,
        .dstArrayElement = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .pImageInfo = &s_block_atlas_descriptor_image_info,
        .pBufferInfo = 0,
        .pTexelBufferView = 0,
      },
    };

    vkUpdateDescriptorSets(g_window.device, ARRAY_COUNT(write_descriptor_set), write_descriptor_set, 0, 0);

    chunk_index++;
  }
}

static void svdb_destroy_buffer(void) {
  buffer_destroy(&s_chunk_info_buffer[0]);
  buffer_destroy(&s_chunk_info_buffer[1]);
  buffer_destroy(&s_chunk_mask_buffer);
  buffer_destroy(&s_select_result_buffer);
  buffer_destroy(&s_place_info_buffer);
  buffer_destroy(&s_place_result_buffer);
  buffer_destroy(&s_block_buffer);

  uint32_t chunk_index = 0;
  uint32_t chunk_count = SVDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    buffer_destroy(&s_chunk_vertex_buffer[chunk_index][0]);
    buffer_destroy(&s_chunk_vertex_buffer[chunk_index][1]);

    buffer_destroy(&s_chunk_index_buffer[chunk_index][0]);
    buffer_destroy(&s_chunk_index_buffer[chunk_index][1]);

    chunk_index++;
  }

  HEAP_FREE(s_chunk_vertex_buffer);
  HEAP_FREE(s_chunk_index_buffer);

  HEAP_FREE(s_chunk_vertex_descriptor_buffer_info);
  HEAP_FREE(s_chunk_index_descriptor_buffer_info);
}
static void svdb_destroy_image(void) {
  image_destroy(&s_block_atlas_image);

  uint32_t chunk_index = 0;
  uint32_t chunk_count = SVDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    image_destroy(&s_chunk_voxel_image[chunk_index]);

    chunk_index++;
  }

  HEAP_FREE(s_chunk_voxel_image);

  HEAP_FREE(s_chunk_voxel_descriptor_image_info);
}
