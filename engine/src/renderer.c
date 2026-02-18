#include <pch.h>

#define CHUNK_SIZE (32)
#define CLUSTER_DIM_X (1)
#define CLUSTER_DIM_Y (1)
#define CHUNK_COUNT (1)

#define DEBUG_LINE_VERTEX_COUNT (0xFFFFF)
#define DEBUG_LINE_INDEX_COUNT (0xFFFFF)

#define MAKE_GROUP_COUNT(GLOBAL_SIZE, LOCAL_SIZE) ((int32_t)ceilf(((float)(GLOBAL_SIZE)) / (LOCAL_SIZE)))

static void renderer_create_sync_object(void);
static void renderer_create_global_buffer(void);
static void renderer_create_debug_line_buffer(void);
static void renderer_create_full_screen_buffer(void);
static void renderer_create_cluster_info_buffer(void);
static void renderer_create_chunk_info_buffer(void);
static void renderer_create_tile_lut_buffer(void);

static void renderer_create_chunk_data_image(void);
static void renderer_create_tile_atlas_image(void);

static void renderer_update_vdb_dda_trace_descriptor_set(void);
static void renderer_update_vdb_world_generator_descriptor_set(void);
static void renderer_update_vdb_iso_renderer_descriptor_set(void);
static void renderer_update_debug_line_descriptor_set(void);

static void renderer_update_uniform_buffer(transform_t *transform, camera_t *camera);

static void renderer_dda_trace(void);
static void renderer_generate_world(void);

static void renderer_record_compute_pass(void);
static void renderer_record_main_pass(void);

static void renderer_destroy_sync_object(void);
static void renderer_destroy_buffer(void);
static void renderer_destroy_image(void);

renderer_t g_renderer = {0};

static int32_t s_image_index = 0;
static int32_t s_debug_line_vertex_offset = 0;
static int32_t s_debug_line_index_offset = 0;

static VkSemaphore s_render_finished_semaphore[SWAPCHAIN_MAX_IMAGE_COUNT] = {0};
static VkSemaphore s_image_available_semaphore = {0};

static VkFence s_frame_fence = {0};

static full_screen_vertex_t const s_full_screen_vertices[] = {
  {-1.0F, -1.0F, 0.0F},
  {1.0F, -1.0F, 0.0F},
  {-1.0F, 1.0F, 0.0F},
  {1.0F, 1.0F, 0.0F},
};
static full_screen_index_t const s_full_screen_indices[] = {
  0,
  1,
  2,
  3,
  2,
  1,
};

static VkVertexInputBindingDescription const s_full_screen_vertex_input_binding_description[] = {
  {
    .binding = 0,
    .stride = sizeof(full_screen_vertex_t),
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
  },
};
static VkVertexInputBindingDescription const s_debug_line_vertex_input_binding_description[] = {
  {
    .binding = 0,
    .stride = sizeof(debug_line_vertex_t),
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
  },
};

static VkVertexInputAttributeDescription const s_full_screen_vertex_input_attribute_description[] = {
  {
    .location = 0,
    .binding = 0,
    .format = VK_FORMAT_R32G32B32_SFLOAT,
    .offset = 0,
  },
};
static VkVertexInputAttributeDescription const s_debug_line_vertex_input_attribute_description[] = {
  {
    .location = 0,
    .binding = 0,
    .format = VK_FORMAT_R32G32B32_SFLOAT,
    .offset = 0,
  },
  {
    .location = 1,
    .binding = 0,
    .format = VK_FORMAT_R32G32B32A32_SFLOAT,
    .offset = OFFSET_OF(debug_line_vertex_t, color),
  },
};

static VkDescriptorPoolSize const s_vdb_dda_trace_descriptor_pool_size[] = {
  {
    .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
  },
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    .descriptorCount = CHUNK_COUNT,
  },
};
static VkDescriptorPoolSize const s_vdb_world_generator_descriptor_pool_size[] = {
  {
    .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
  },
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    .descriptorCount = CHUNK_COUNT,
  },
};
static VkDescriptorPoolSize const s_vdb_iso_renderer_descriptor_pool_size[] = {
  {
    .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
  },
  {
    .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    .descriptorCount = 1 + CHUNK_COUNT,
  },
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 1,
  },
};
static VkDescriptorPoolSize const s_vdb_debug_line_renderer_descriptor_pool_size[] = {
  {
    .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
  },
};

static VkDescriptorSetLayoutBinding const s_vdb_dda_trace_descriptor_set_layout_binding[] = {
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
    .descriptorCount = CHUNK_COUNT,
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .pImmutableSamplers = 0,
  },
};
static VkDescriptorSetLayoutBinding const s_vdb_world_generator_descriptor_set_layout_binding[] = {
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
    .descriptorCount = CHUNK_COUNT,
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .pImmutableSamplers = 0,
  },
};
static VkDescriptorSetLayoutBinding const s_vdb_iso_renderer_descriptor_set_layout_binding[] = {
  {
    .binding = 0,
    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_TASK_BIT_EXT | VK_SHADER_STAGE_MESH_BIT_EXT,
    .pImmutableSamplers = 0,
  },
  {
    .binding = 1,
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_MESH_BIT_EXT,
    .pImmutableSamplers = 0,
  },
  {
    .binding = 2,
    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    .descriptorCount = CHUNK_COUNT,
    .stageFlags = VK_SHADER_STAGE_MESH_BIT_EXT,
    .pImmutableSamplers = 0,
  },
  {
    .binding = 3,
    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
    .pImmutableSamplers = 0,
  },
};
static VkDescriptorSetLayoutBinding const s_debug_line_renderer_descriptor_set_layout_binding[] = {
  {
    .binding = 0,
    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    .pImmutableSamplers = 0,
  },
};

static VkPushConstantRange const s_vdb_iso_renderer_push_constant_range[] = {
  {
    .stageFlags = VK_SHADER_STAGE_MESH_BIT_EXT | VK_SHADER_STAGE_FRAGMENT_BIT,
    .offset = 0,
    .size = sizeof(iso_renderer_push_constant_t),
  },
};

static pipeline_t s_vdb_dda_trace_pipeline = {
  .pipeline_type = PIPELINE_TYPE_C,
  .compute_shader = ROOT_DIR "/shader/vdb/dda_trace.comp.spv",
  .descriptor_pool_size = s_vdb_dda_trace_descriptor_pool_size,
  .descriptor_pool_size_count = ARRAY_COUNT(s_vdb_dda_trace_descriptor_pool_size),
  .descriptor_set_layout_binding = s_vdb_dda_trace_descriptor_set_layout_binding,
  .descriptor_set_layout_binding_count = ARRAY_COUNT(s_vdb_dda_trace_descriptor_set_layout_binding),
};
static pipeline_t s_vdb_world_generator_pipeline = {
  .pipeline_type = PIPELINE_TYPE_C,
  .compute_shader = ROOT_DIR "/shader/vdb/world_generator.comp.spv",
  .descriptor_pool_size = s_vdb_world_generator_descriptor_pool_size,
  .descriptor_pool_size_count = ARRAY_COUNT(s_vdb_world_generator_descriptor_pool_size),
  .descriptor_set_layout_binding = s_vdb_world_generator_descriptor_set_layout_binding,
  .descriptor_set_layout_binding_count = ARRAY_COUNT(s_vdb_world_generator_descriptor_set_layout_binding),
};
static pipeline_t s_vdb_iso_renderer_pipeline = {
  .pipeline_type = PIPELINE_TYPE_TMF,
  .task_shader = ROOT_DIR "/shader/vdb/iso_renderer.task.spv",
  .mesh_shader = ROOT_DIR "/shader/vdb/iso_renderer.mesh.spv",
  .fragment_shader = ROOT_DIR "/shader/vdb/iso_renderer.frag.spv",
  .push_constant_range = s_vdb_iso_renderer_push_constant_range,
  .push_constant_range_count = ARRAY_COUNT(s_vdb_iso_renderer_push_constant_range),
  .descriptor_pool_size = s_vdb_iso_renderer_descriptor_pool_size,
  .descriptor_pool_size_count = ARRAY_COUNT(s_vdb_iso_renderer_descriptor_pool_size),
  .descriptor_set_layout_binding = s_vdb_iso_renderer_descriptor_set_layout_binding,
  .descriptor_set_layout_binding_count = ARRAY_COUNT(s_vdb_iso_renderer_descriptor_set_layout_binding),
  .render_pass = &g_renderpass_main,
};
static pipeline_t s_debug_line_renderer_pipeline = {
  .pipeline_type = PIPELINE_TYPE_VF,
  .vertex_shader = ROOT_DIR "/shader/debug/line_renderer.vert.spv",
  .fragment_shader = ROOT_DIR "/shader/debug/line_renderer.frag.spv",
  .vertex_input_binding_description = s_debug_line_vertex_input_binding_description,
  .vertex_input_binding_description_count = ARRAY_COUNT(s_debug_line_vertex_input_binding_description),
  .vertex_input_attribute_description = s_debug_line_vertex_input_attribute_description,
  .vertex_input_attribute_description_count = ARRAY_COUNT(s_debug_line_vertex_input_attribute_description),
  .descriptor_pool_size = s_vdb_debug_line_renderer_descriptor_pool_size,
  .descriptor_pool_size_count = ARRAY_COUNT(s_vdb_debug_line_renderer_descriptor_pool_size),
  .descriptor_set_layout_binding = s_debug_line_renderer_descriptor_set_layout_binding,
  .descriptor_set_layout_binding_count = ARRAY_COUNT(s_debug_line_renderer_descriptor_set_layout_binding),
  .render_pass = &g_renderpass_main,
};

static buffer_t s_debug_line_vertex_buffer = {
  .size = sizeof(debug_line_vertex_t) * DEBUG_LINE_VERTEX_COUNT,
  .buffer_usage_flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
};
static buffer_t s_debug_line_index_buffer = {
  .size = sizeof(debug_line_index_t) * DEBUG_LINE_INDEX_COUNT,
  .buffer_usage_flags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
};
static buffer_t s_full_screen_vertex_buffer = {
  .host_data = (void *)s_full_screen_vertices,
  .size = sizeof(s_full_screen_vertices),
  .buffer_usage_flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
};
static buffer_t s_full_screen_index_buffer = {
  .host_data = (void *)s_full_screen_indices,
  .size = sizeof(s_full_screen_indices),
  .buffer_usage_flags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
};
static buffer_t s_time_info_buffer = {
  .size = sizeof(time_info_t),
  .buffer_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
};
static buffer_t s_screen_info_buffer = {
  .size = sizeof(screen_info_t),
  .buffer_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
};
static buffer_t s_camera_info_buffer = {
  .size = sizeof(camera_info_t),
  .buffer_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
};
static buffer_t s_cluster_info_buffer = {
  .size = sizeof(cluster_info_t),
  .buffer_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
};
static buffer_t s_chunk_info_buffer = {
  .size = sizeof(chunk_info_t) * CHUNK_COUNT,
  .buffer_usage_flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
};
static buffer_t s_tile_lut_buffer = {
  .host_data = (void *)g_tile_lut,
  .size = sizeof(g_tile_lut),
  .buffer_usage_flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
};

static image_t *s_chunk_data_image = 0;
static image_t s_tile_atlas_image = {
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

static VkDescriptorBufferInfo s_time_info_descriptor_buffer_info = {0};
static VkDescriptorBufferInfo s_screen_info_descriptor_buffer_info = {0};
static VkDescriptorBufferInfo s_camera_info_descriptor_buffer_info = {0};
static VkDescriptorBufferInfo s_cluster_info_descriptor_buffer_info = {0};
static VkDescriptorBufferInfo s_chunk_info_descriptor_buffer_info = {0};
static VkDescriptorBufferInfo s_tile_lut_descriptor_buffer_info = {0};

static VkDescriptorImageInfo *s_chunk_data_descriptor_image_info = 0;
static VkDescriptorImageInfo s_tile_atlas_descriptor_image_info = {0};

void renderer_create(void) {
  g_renderer.is_debug_enabled = 1;
  g_renderer.rebuild_world = 1;
  g_renderer.tile_size = 0.0F;
  g_renderer.tile_height = 0.0F;
  g_renderer.inner_scale = 64.0F;
  g_renderer.outer_scale = 64.0F;
  g_renderer.rotation.x = 0.0F;
  g_renderer.rotation.y = 45.0F;
  g_renderer.rotation.z = 180.0F;

  renderer_create_sync_object();
  renderer_create_global_buffer();
  renderer_create_debug_line_buffer();
  renderer_create_full_screen_buffer();
  renderer_create_cluster_info_buffer();
  renderer_create_chunk_info_buffer();
  renderer_create_tile_lut_buffer();

  renderer_create_chunk_data_image();
  renderer_create_tile_atlas_image();

  pipeline_create(&s_vdb_dda_trace_pipeline);
  pipeline_create(&s_vdb_world_generator_pipeline);
  pipeline_create(&s_vdb_iso_renderer_pipeline);
  pipeline_create(&s_debug_line_renderer_pipeline);

  renderer_update_vdb_dda_trace_descriptor_set();
  renderer_update_vdb_world_generator_descriptor_set();
  renderer_update_vdb_iso_renderer_descriptor_set();
  renderer_update_debug_line_descriptor_set();

  dbgui_create();
}
void renderer_draw(transform_t *transform, camera_t *camera) {
  VkResult result = VK_SUCCESS;

  result = vkWaitForFences(g_window.device, 1, &s_frame_fence, 1, UINT64_MAX);

  switch (result) {
    case VK_SUCCESS: {
      break;
    }
#ifdef BUILD_DEBUG
    default: {
      __debugbreak();
    }
#endif // BUILD_DEBUG
  }

  result = vkResetFences(g_window.device, 1, &s_frame_fence);

  switch (result) {
    case VK_SUCCESS: {
      break;
    }
#ifdef BUILD_DEBUG
    default: {
      __debugbreak();
    }
#endif // BUILD_DEBUG
  }

  result = vkResetCommandBuffer(g_window.command_buffer, 0);

  switch (result) {
    case VK_SUCCESS: {
      break;
    }
#ifdef BUILD_DEBUG
    default: {
      __debugbreak();
    }
#endif // BUILD_DEBUG
  }

  result = vkAcquireNextImageKHR(g_window.device, g_swapchain.handle, UINT64_MAX, s_image_available_semaphore, 0, &s_image_index);

  switch (result) {
    case VK_SUCCESS: {
      break;
    }
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_SUBOPTIMAL_KHR: {

      g_swapchain.is_dirty = 1;

      return;
    }
#ifdef BUILD_DEBUG
    default: {
      __debugbreak();
    }
#endif // BUILD_DEBUG
  }

  renderer_update_uniform_buffer(transform, camera);

  VkCommandBufferBeginInfo command_buffer_begin_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    .pInheritanceInfo = 0,
  };

  VK_CHECK(vkBeginCommandBuffer(g_window.command_buffer, &command_buffer_begin_info));

  renderer_record_compute_pass();
  renderer_record_main_pass();

  VK_CHECK(vkEndCommandBuffer(g_window.command_buffer));

  VkPipelineStageFlags graphics_wait_stages[] = {
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
  };

  VkSubmitInfo graphics_submit_info = {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .pWaitSemaphores = &s_image_available_semaphore,
    .waitSemaphoreCount = 1,
    .pSignalSemaphores = &s_render_finished_semaphore[s_image_index],
    .signalSemaphoreCount = 1,
    .pCommandBuffers = &g_window.command_buffer,
    .commandBufferCount = 1,
    .pWaitDstStageMask = graphics_wait_stages,
  };

  result = vkQueueSubmit(g_window.primary_queue, 1, &graphics_submit_info, s_frame_fence);

  switch (result) {
    case VK_SUCCESS: {
      break;
    }
#ifdef BUILD_DEBUG
    default: {
      __debugbreak();
    }
#endif // BUILD_DEBUG
  }

  VkPresentInfoKHR present_info = {
    .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
    .pWaitSemaphores = &s_render_finished_semaphore[s_image_index],
    .waitSemaphoreCount = 1,
    .pSwapchains = &g_swapchain.handle,
    .swapchainCount = 1,
    .pImageIndices = &s_image_index,
  };

  result = vkQueuePresentKHR(g_window.present_queue, &present_info);

  switch (result) {
    case VK_SUCCESS: {
      break;
    }
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_SUBOPTIMAL_KHR: {

      g_swapchain.is_dirty = 1;

      return;
    }
#ifdef BUILD_DEBUG
    default: {
      __debugbreak();
    }
#endif // BUILD_DEBUG
  }
}
void renderer_destroy(void) {
  dbgui_destroy();

  pipeline_destroy(&s_vdb_dda_trace_pipeline);
  pipeline_destroy(&s_vdb_world_generator_pipeline);
  pipeline_destroy(&s_vdb_iso_renderer_pipeline);
  pipeline_destroy(&s_debug_line_renderer_pipeline);

  renderer_destroy_sync_object();
  renderer_destroy_buffer();
  renderer_destroy_image();
}

void renderer_draw_debug_line(vector3_t from, vector3_t to, vector4_t color) {
  if (g_renderer.is_debug_enabled) {

    uint32_t vertex_offset = s_debug_line_vertex_offset;
    uint32_t index_offset = s_debug_line_index_offset;

    if ((vertex_offset + 2) < DEBUG_LINE_VERTEX_COUNT &&
        (index_offset + 2) < DEBUG_LINE_INDEX_COUNT) {

      debug_line_vertex_t *vertices = (debug_line_vertex_t *)s_debug_line_vertex_buffer.device_data;
      debug_line_index_t *indices = (debug_line_index_t *)s_debug_line_index_buffer.device_data;

      vertices[vertex_offset + 0].position = from;
      vertices[vertex_offset + 1].position = to;

      vertices[vertex_offset + 0].color = color;
      vertices[vertex_offset + 1].color = color;

      indices[index_offset + 0] = (debug_line_index_t)(vertex_offset + 0);
      indices[index_offset + 1] = (debug_line_index_t)(vertex_offset + 1);

      s_debug_line_vertex_offset += 2;
      s_debug_line_index_offset += 2;
    }
  }
}
void renderer_draw_debug_box(vector3_t position, vector3_t size, vector4_t color) {
  if (g_renderer.is_debug_enabled) {

    uint32_t vertex_offset = s_debug_line_vertex_offset;
    uint32_t index_offset = s_debug_line_index_offset;

    if ((vertex_offset + 8) < DEBUG_LINE_VERTEX_COUNT &&
        (index_offset + 24) < DEBUG_LINE_INDEX_COUNT) {

      debug_line_vertex_t *vertices = (debug_line_vertex_t *)s_debug_line_vertex_buffer.device_data;
      debug_line_index_t *indices = (debug_line_index_t *)s_debug_line_index_buffer.device_data;

      vertices[vertex_offset + 0].position = (vector3_t){position.x, position.y, position.z};
      vertices[vertex_offset + 1].position = (vector3_t){position.x, position.y + size.y, position.z};
      vertices[vertex_offset + 2].position = (vector3_t){position.x + size.x, position.y, position.z};
      vertices[vertex_offset + 3].position = (vector3_t){position.x + size.x, position.y + size.y, position.z};
      vertices[vertex_offset + 4].position = (vector3_t){position.x, position.y, position.z + size.z};
      vertices[vertex_offset + 5].position = (vector3_t){position.x, position.y + size.y, position.z + size.z};
      vertices[vertex_offset + 6].position = (vector3_t){position.x + size.x, position.y, position.z + size.z};
      vertices[vertex_offset + 7].position = (vector3_t){position.x + size.x, position.y + size.y, position.z + size.z};

      vertices[vertex_offset + 0].color = color;
      vertices[vertex_offset + 1].color = color;
      vertices[vertex_offset + 2].color = color;
      vertices[vertex_offset + 3].color = color;
      vertices[vertex_offset + 4].color = color;
      vertices[vertex_offset + 5].color = color;
      vertices[vertex_offset + 6].color = color;
      vertices[vertex_offset + 7].color = color;

      indices[index_offset + 0] = (debug_line_index_t)(vertex_offset + 0);
      indices[index_offset + 1] = (debug_line_index_t)(vertex_offset + 1);
      indices[index_offset + 2] = (debug_line_index_t)(vertex_offset + 1);
      indices[index_offset + 3] = (debug_line_index_t)(vertex_offset + 3);
      indices[index_offset + 4] = (debug_line_index_t)(vertex_offset + 3);
      indices[index_offset + 5] = (debug_line_index_t)(vertex_offset + 2);
      indices[index_offset + 6] = (debug_line_index_t)(vertex_offset + 2);
      indices[index_offset + 7] = (debug_line_index_t)(vertex_offset + 0);
      indices[index_offset + 8] = (debug_line_index_t)(vertex_offset + 4);
      indices[index_offset + 9] = (debug_line_index_t)(vertex_offset + 5);
      indices[index_offset + 10] = (debug_line_index_t)(vertex_offset + 5);
      indices[index_offset + 11] = (debug_line_index_t)(vertex_offset + 7);
      indices[index_offset + 12] = (debug_line_index_t)(vertex_offset + 7);
      indices[index_offset + 13] = (debug_line_index_t)(vertex_offset + 6);
      indices[index_offset + 14] = (debug_line_index_t)(vertex_offset + 6);
      indices[index_offset + 15] = (debug_line_index_t)(vertex_offset + 4);
      indices[index_offset + 16] = (debug_line_index_t)(vertex_offset + 0);
      indices[index_offset + 17] = (debug_line_index_t)(vertex_offset + 4);
      indices[index_offset + 18] = (debug_line_index_t)(vertex_offset + 1);
      indices[index_offset + 19] = (debug_line_index_t)(vertex_offset + 5);
      indices[index_offset + 20] = (debug_line_index_t)(vertex_offset + 2);
      indices[index_offset + 21] = (debug_line_index_t)(vertex_offset + 6);
      indices[index_offset + 22] = (debug_line_index_t)(vertex_offset + 3);
      indices[index_offset + 23] = (debug_line_index_t)(vertex_offset + 7);

      s_debug_line_vertex_offset += 8;
      s_debug_line_index_offset += 24;
    }
  }
}

static void renderer_create_sync_object(void) {
  VkSemaphoreCreateInfo semaphore_create_info = {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    .flags = 0,
  };

  VkFenceCreateInfo fence_create_info = {
    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    .flags = VK_FENCE_CREATE_SIGNALED_BIT,
  };

  int32_t image_index = 0;
  int32_t image_count = g_swapchain.image_count;

  while (image_index < image_count) {

    VK_CHECK(vkCreateSemaphore(g_window.device, &semaphore_create_info, 0, &s_render_finished_semaphore[image_index]));

    image_index++;
  }

  VK_CHECK(vkCreateSemaphore(g_window.device, &semaphore_create_info, 0, &s_image_available_semaphore));
  VK_CHECK(vkCreateFence(g_window.device, &fence_create_info, 0, &s_frame_fence));
}
static void renderer_create_global_buffer(void) {
  buffer_create(&s_time_info_buffer);
  buffer_create(&s_screen_info_buffer);
  buffer_create(&s_camera_info_buffer);

  buffer_map(&s_time_info_buffer);
  buffer_map(&s_screen_info_buffer);
  buffer_map(&s_camera_info_buffer);

  s_time_info_descriptor_buffer_info.offset = 0;
  s_time_info_descriptor_buffer_info.buffer = s_time_info_buffer.buffer_handle;
  s_time_info_descriptor_buffer_info.range = VK_WHOLE_SIZE;

  s_screen_info_descriptor_buffer_info.offset = 0;
  s_screen_info_descriptor_buffer_info.buffer = s_screen_info_buffer.buffer_handle;
  s_screen_info_descriptor_buffer_info.range = VK_WHOLE_SIZE;

  s_camera_info_descriptor_buffer_info.offset = 0;
  s_camera_info_descriptor_buffer_info.buffer = s_camera_info_buffer.buffer_handle;
  s_camera_info_descriptor_buffer_info.range = VK_WHOLE_SIZE;
}
static void renderer_create_debug_line_buffer(void) {
  buffer_create(&s_debug_line_vertex_buffer);
  buffer_create(&s_debug_line_index_buffer);

  buffer_map(&s_debug_line_vertex_buffer);
  buffer_map(&s_debug_line_index_buffer);
}
static void renderer_create_full_screen_buffer(void) {
  buffer_create(&s_full_screen_vertex_buffer);
  buffer_create(&s_full_screen_index_buffer);
}
static void renderer_create_cluster_info_buffer(void) {
  buffer_create(&s_cluster_info_buffer);

  buffer_map(&s_cluster_info_buffer);

  s_cluster_info_descriptor_buffer_info.offset = 0;
  s_cluster_info_descriptor_buffer_info.buffer = s_cluster_info_buffer.buffer_handle;
  s_cluster_info_descriptor_buffer_info.range = VK_WHOLE_SIZE;
}
static void renderer_create_chunk_info_buffer(void) {
  chunk_info_t *chunk_info = (chunk_info_t *)HEAP_ALLOC(sizeof(chunk_info_t) * CHUNK_COUNT, 1, 0);

  int32_t chunk_index = 0;
  int32_t chunk_count = CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    chunk_info[chunk_index].chunk_position = (ivector2_t){chunk_index % CLUSTER_DIM_X, (chunk_index / CLUSTER_DIM_X) % CLUSTER_DIM_Y};
    chunk_info[chunk_index].chunk_size = (ivector2_t){CHUNK_SIZE, CHUNK_SIZE};

    chunk_index++;
  }

  s_chunk_info_buffer.host_data = chunk_info;

  buffer_create(&s_chunk_info_buffer);

  s_chunk_info_buffer.host_data = 0;

  HEAP_FREE(chunk_info);

  s_chunk_info_descriptor_buffer_info.offset = 0;
  s_chunk_info_descriptor_buffer_info.buffer = s_chunk_info_buffer.buffer_handle;
  s_chunk_info_descriptor_buffer_info.range = VK_WHOLE_SIZE;
}
static void renderer_create_tile_lut_buffer(void) {
  buffer_create(&s_tile_lut_buffer);

  s_tile_lut_descriptor_buffer_info.offset = 0;
  s_tile_lut_descriptor_buffer_info.buffer = s_tile_lut_buffer.buffer_handle;
  s_tile_lut_descriptor_buffer_info.range = VK_WHOLE_SIZE;
}

static void renderer_create_chunk_data_image(void) {
  s_chunk_data_image = (image_t *)HEAP_ALLOC(sizeof(image_t) * CHUNK_COUNT, 1, 0);
  s_chunk_data_descriptor_image_info = (VkDescriptorImageInfo *)HEAP_ALLOC(sizeof(VkDescriptorImageInfo) * CHUNK_COUNT, 1, 0);

  int32_t chunk_index = 0;
  int32_t chunk_count = CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    s_chunk_data_image[chunk_index].width = CHUNK_SIZE,
    s_chunk_data_image[chunk_index].height = CHUNK_SIZE,
    s_chunk_data_image[chunk_index].depth = CHUNK_SIZE,
    s_chunk_data_image[chunk_index].channel = 1,
    s_chunk_data_image[chunk_index].element_size = sizeof(uint32_t),
    s_chunk_data_image[chunk_index].format = VK_FORMAT_R32_UINT,
    s_chunk_data_image[chunk_index].filter = VK_FILTER_NEAREST,
    s_chunk_data_image[chunk_index].image_usage_flags = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
    s_chunk_data_image[chunk_index].image_type = VK_IMAGE_TYPE_3D,
    s_chunk_data_image[chunk_index].image_view_type = VK_IMAGE_VIEW_TYPE_3D,
    s_chunk_data_image[chunk_index].image_aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT,
    s_chunk_data_image[chunk_index].image_tiling = VK_IMAGE_TILING_OPTIMAL,

    image_create(&s_chunk_data_image[chunk_index]);

    s_chunk_data_descriptor_image_info[chunk_index].sampler = s_chunk_data_image[chunk_index].sampler;
    s_chunk_data_descriptor_image_info[chunk_index].imageView = s_chunk_data_image[chunk_index].image_view;
    s_chunk_data_descriptor_image_info[chunk_index].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    chunk_index++;
  }
}
static void renderer_create_tile_atlas_image(void) {
  uint8_t *image = imgutil_load_image_from_file(0, 0, 0, ROOT_DIR "/asset/terrain.png");

  s_tile_atlas_image.host_data = image;

  image_create(&s_tile_atlas_image);

  s_tile_atlas_image.host_data = 0;

  imgutil_free_image(image);

  s_tile_atlas_descriptor_image_info.sampler = s_tile_atlas_image.sampler;
  s_tile_atlas_descriptor_image_info.imageView = s_tile_atlas_image.image_view;
  s_tile_atlas_descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
}

static void renderer_update_vdb_dda_trace_descriptor_set(void) {
  VkWriteDescriptorSet write_descriptor_set[] = {
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_vdb_dda_trace_pipeline.descriptor_set,
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
      .dstSet = s_vdb_dda_trace_pipeline.descriptor_set,
      .dstBinding = 1,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
      .descriptorCount = CHUNK_COUNT,
      .pImageInfo = s_chunk_data_descriptor_image_info,
      .pBufferInfo = 0,
      .pTexelBufferView = 0,
    },
  };

  vkUpdateDescriptorSets(g_window.device, ARRAY_COUNT(write_descriptor_set), write_descriptor_set, 0, 0);
}
static void renderer_update_vdb_world_generator_descriptor_set(void) {
  VkWriteDescriptorSet write_descriptor_set[] = {
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_vdb_world_generator_pipeline.descriptor_set,
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
      .dstSet = s_vdb_world_generator_pipeline.descriptor_set,
      .dstBinding = 1,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
      .descriptorCount = CHUNK_COUNT,
      .pImageInfo = s_chunk_data_descriptor_image_info,
      .pBufferInfo = 0,
      .pTexelBufferView = 0,
    },
  };

  vkUpdateDescriptorSets(g_window.device, ARRAY_COUNT(write_descriptor_set), write_descriptor_set, 0, 0);
}
static void renderer_update_vdb_iso_renderer_descriptor_set(void) {
  VkWriteDescriptorSet write_descriptor_set[] = {
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_vdb_iso_renderer_pipeline.descriptor_set,
      .dstBinding = 0,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = 1,
      .pImageInfo = 0,
      .pBufferInfo = &s_camera_info_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_vdb_iso_renderer_pipeline.descriptor_set,
      .dstBinding = 1,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      .descriptorCount = 1,
      .pImageInfo = 0,
      .pBufferInfo = &s_tile_lut_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_vdb_iso_renderer_pipeline.descriptor_set,
      .dstBinding = 2,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .descriptorCount = CHUNK_COUNT,
      .pImageInfo = s_chunk_data_descriptor_image_info,
      .pBufferInfo = 0,
      .pTexelBufferView = 0,
    },
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_vdb_iso_renderer_pipeline.descriptor_set,
      .dstBinding = 3,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .descriptorCount = 1,
      .pImageInfo = &s_tile_atlas_descriptor_image_info,
      .pBufferInfo = 0,
      .pTexelBufferView = 0,
    },
  };

  vkUpdateDescriptorSets(g_window.device, ARRAY_COUNT(write_descriptor_set), write_descriptor_set, 0, 0);
}
static void renderer_update_debug_line_descriptor_set(void) {
  VkWriteDescriptorSet write_descriptor_set[] = {
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_debug_line_renderer_pipeline.descriptor_set,
      .dstBinding = 0,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = 1,
      .pImageInfo = 0,
      .pBufferInfo = &s_camera_info_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
  };

  vkUpdateDescriptorSets(g_window.device, ARRAY_COUNT(write_descriptor_set), write_descriptor_set, 0, 0);
}

static void renderer_update_uniform_buffer(transform_t *transform, camera_t *camera) {
  time_info_t *time_info = (time_info_t *)s_time_info_buffer.device_data;
  time_info->time = g_window.time;
  time_info->delta_time = g_window.delta_time;

  screen_info_t *screen_info = (screen_info_t *)s_screen_info_buffer.device_data;
  screen_info->resolution = vector2_xy((float)g_window.window_width, (float)g_window.window_height);

  camera_info_t *camera_info = (camera_info_t *)s_camera_info_buffer.device_data;
  camera_info->position = transform->world_position;
  camera_info->view = camera->view;
  camera_info->projection = camera->projection;
  camera_info->view_projection = camera->view_projection;
  camera_info->view_projection_inv = camera->view_projection_inv;

  cluster_info_t *cluster_info = (cluster_info_t *)s_cluster_info_buffer.device_data;
  cluster_info->dimension = (ivector2_t){CLUSTER_DIM_X, CLUSTER_DIM_Y};
}

static void renderer_generate_world(void) {
  vkCmdBindPipeline(g_window.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_vdb_world_generator_pipeline.pipeline_handle);
  vkCmdBindDescriptorSets(g_window.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_vdb_world_generator_pipeline.pipeline_layout, 0, 1, &s_vdb_world_generator_pipeline.descriptor_set, 0, 0);

  int32_t group_count = MAKE_GROUP_COUNT(CHUNK_SIZE, 8);
  int32_t chunk_index = 0;
  int32_t chunk_count = CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    vkCmdDispatch(g_window.command_buffer, group_count, group_count, group_count);

    /// TODO: make image memory barriere..
    // VkBufferMemoryBarrier buffer_memory_barrier = {
    //   .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
    //   .pNext = 0,
    //   .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
    //   .dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
    //   .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    //   .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    //   .buffer = s_chunk_mask_buffer.handle,
    //   .offset = 0,
    //   .size = VK_WHOLE_SIZE,
    // };

    // vkCmdPipelineBarrier(
    //   g_window.command_buffer,
    //   VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
    //   VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
    //   0,
    //   0,
    //   0,
    //   1,
    //   &buffer_memory_barrier,
    //   0,
    //   0);

    chunk_index++;
  }
}
static void renderer_dda_trace(void) {
  int32_t group_count = 1;

  vkCmdBindPipeline(g_window.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_vdb_dda_trace_pipeline.pipeline_handle);
  vkCmdBindDescriptorSets(g_window.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, s_vdb_dda_trace_pipeline.pipeline_layout, 0, 1, &s_vdb_dda_trace_pipeline.descriptor_set, 0, 0);
  vkCmdDispatch(g_window.command_buffer, group_count, group_count, group_count);
}

static void renderer_record_compute_pass(void) {
  if (g_renderer.rebuild_world) {

    g_renderer.rebuild_world = 0;

    renderer_generate_world();
  }

  if (window_is_mouse_key_held(MOUSE_KEY_RIGHT)) {

    renderer_dda_trace();
  }
}
static void renderer_record_main_pass(void) {
  VkClearValue color_clear_value = {
    .color.float32 = {
      0.0F,
      0.0F,
      0.0F,
      1.0F,
    },
  };

  VkClearValue depth_clear_value = {
    .depthStencil = {
      .depth = 1.0F,
      .stencil = 0,
    },
  };

  VkClearValue clear_values[] = {
    color_clear_value,
    depth_clear_value,
  };

  VkRenderPassBeginInfo render_pass_create_info = {
    .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
    .renderPass = g_renderpass_main,
    .framebuffer = g_framebuffer_main.handle[s_image_index],
    .renderArea = {
      .offset.x = 0,
      .offset.y = 0,
      .extent = {
        .width = g_window.window_width,
        .height = g_window.window_height,
      },
    },
    .pClearValues = clear_values,
    .clearValueCount = ARRAY_COUNT(clear_values),
  };

  vkCmdBeginRenderPass(g_window.command_buffer, &render_pass_create_info, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport = {
    .x = 0.0F,
    .y = 0.0F,
    .width = (float)g_window.window_width,
    .height = (float)g_window.window_height,
    .minDepth = 0.0F,
    .maxDepth = 1.0F,
  };

  vkCmdSetViewport(g_window.command_buffer, 0, 1, &viewport);

  VkRect2D scissor = {
    .offset.x = 0,
    .offset.y = 0,
    .extent = {
      .width = g_window.window_width,
      .height = g_window.window_height,
    },
  };

  vkCmdSetScissor(g_window.command_buffer, 0, 1, &scissor);

  {
    int32_t group_count = 1;

    iso_renderer_push_constant_t iso_renderer_push_constant = {
      .vertex_offset_0 = g_renderer.vertex_offset_0,
      .vertex_offset_1 = g_renderer.vertex_offset_1,
      .vertex_offset_2 = g_renderer.vertex_offset_2,
      .vertex_offset_3 = g_renderer.vertex_offset_3,
      .rotation = g_renderer.rotation,
      .tile_size = g_renderer.tile_size,
      .tile_height = g_renderer.tile_height,
      .inner_scale = g_renderer.inner_scale,
      .outer_scale = g_renderer.outer_scale,
    };

    vkCmdBindPipeline(g_window.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, s_vdb_iso_renderer_pipeline.pipeline_handle);
    vkCmdBindDescriptorSets(g_window.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, s_vdb_iso_renderer_pipeline.pipeline_layout, 0, 1, &s_vdb_iso_renderer_pipeline.descriptor_set, 0, 0);
    vkCmdPushConstants(g_window.command_buffer, s_vdb_iso_renderer_pipeline.pipeline_layout, VK_SHADER_STAGE_MESH_BIT_EXT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(iso_renderer_push_constant_t), &iso_renderer_push_constant);
    vkCmdDrawMeshTasks(g_window.command_buffer, group_count, group_count, group_count);
  }

  {
    if (g_renderer.is_debug_enabled) {

      VkBuffer vertex_buffer[] = {s_debug_line_vertex_buffer.buffer_handle};
      VkDeviceSize vertex_offset[] = {0};

      vkCmdBindPipeline(g_window.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, s_debug_line_renderer_pipeline.pipeline_handle);
      vkCmdBindVertexBuffers(g_window.command_buffer, 0, ARRAY_COUNT(vertex_buffer), vertex_buffer, vertex_offset);
      vkCmdBindIndexBuffer(g_window.command_buffer, s_debug_line_index_buffer.buffer_handle, 0, VK_INDEX_TYPE_UINT32);
      vkCmdBindDescriptorSets(g_window.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, s_debug_line_renderer_pipeline.pipeline_layout, 0, 1, &s_debug_line_renderer_pipeline.descriptor_set, 0, 0);
      vkCmdDrawIndexed(g_window.command_buffer, s_debug_line_index_offset, 1, 0, 0, 0);

      s_debug_line_vertex_offset = 0;
      s_debug_line_index_offset = 0;
    }
  }

  dbgui_draw();

  vkCmdEndRenderPass(g_window.command_buffer);
}

static void renderer_destroy_sync_object(void) {
  int32_t image_index = 0;
  int32_t image_count = g_swapchain.image_count;

  while (image_index < image_count) {

    vkDestroySemaphore(g_window.device, s_render_finished_semaphore[image_index], 0);

    image_index++;
  }

  vkDestroySemaphore(g_window.device, s_image_available_semaphore, 0);
  vkDestroyFence(g_window.device, s_frame_fence, 0);
}
static void renderer_destroy_buffer(void) {
  buffer_destroy(&s_debug_line_vertex_buffer);
  buffer_destroy(&s_debug_line_index_buffer);
  buffer_destroy(&s_full_screen_vertex_buffer);
  buffer_destroy(&s_full_screen_index_buffer);
  buffer_destroy(&s_time_info_buffer);
  buffer_destroy(&s_screen_info_buffer);
  buffer_destroy(&s_camera_info_buffer);
  buffer_destroy(&s_cluster_info_buffer);
  buffer_destroy(&s_chunk_info_buffer);
  buffer_destroy(&s_tile_lut_buffer);
}
static void renderer_destroy_image(void) {
  int32_t chunk_index = 0;
  int32_t chunk_count = CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    image_destroy(&s_chunk_data_image[chunk_index]);

    chunk_index++;
  }

  HEAP_FREE(s_chunk_data_image);
  HEAP_FREE(s_chunk_data_descriptor_image_info);

  image_destroy(&s_tile_atlas_image);
}
