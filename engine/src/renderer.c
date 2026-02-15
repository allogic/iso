#include <pch.h>

#define MAKE_GROUP_COUNT(GLOBAL_SIZE, LOCAL_SIZE) ((int32_t)ceilf(((float)(GLOBAL_SIZE)) / (LOCAL_SIZE)))

static void renderer_create_sync_object(void);
static void renderer_create_descriptor_pool(void);
static void renderer_create_descriptor_set_layout(void);
static void renderer_create_descriptor_set(void);
static void renderer_create_buffer(void);
static void renderer_create_pipeline_layout(void);

static void renderer_create_vdb_world_generator_pipeline(char const *compute_shader_file_path);
static void renderer_create_vdb_geom_renderer_pipeline(char const *task_shader_file_path, char const *mesh_shader_file_path, char const *fragment_shader_file_path);
static void renderer_create_debug_line_pipeline(char const *vertex_shader_file_path, char const *fragment_shader_file_path);

static void renderer_update_vdb_world_generator_descriptor_set(void);
static void renderer_update_vdb_geom_renderer_descriptor_set(void);
static void renderer_update_debug_line_descriptor_set(void);

static void renderer_compute_world(void);

static void renderer_update_uniform_buffer(transform_t *transform, camera_t *camera);

static void renderer_record_compute_pass(void);
static void renderer_record_main_pass(void);

static void renderer_destroy_sync_object(void);
static void renderer_destroy_descriptor_pool(void);
static void renderer_destroy_descriptor_set_layout(void);
static void renderer_destroy_buffer(void);
static void renderer_destroy_pipeline_layout(void);
static void renderer_destroy_pipeline(void);

renderer_t g_renderer = {0};

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

void renderer_create(void) {
  g_renderer.is_debug_enabled = 1;
  g_renderer.rebuild_world = 1;

  renderer_create_sync_object();
  renderer_create_descriptor_pool();
  renderer_create_descriptor_set_layout();
  renderer_create_descriptor_set();
  renderer_create_buffer();
  renderer_create_pipeline_layout();

  renderer_create_vdb_world_generator_pipeline(ROOT_DIR "/shader/vdb/world_generator.comp.spv");
  renderer_create_vdb_geom_renderer_pipeline(ROOT_DIR "/shader/vdb/geom_renderer.task.spv", ROOT_DIR "/shader/vdb/geom_renderer.mesh.spv", ROOT_DIR "/shader/vdb/geom_renderer.frag.spv");
  renderer_create_debug_line_pipeline(ROOT_DIR "/shader/debug/line_renderer.vert.spv", ROOT_DIR "/shader/debug/line_renderer.frag.spv");

  renderer_update_vdb_world_generator_descriptor_set();
  renderer_update_vdb_geom_renderer_descriptor_set();
  renderer_update_debug_line_descriptor_set();

  dbgui_create();
}
void renderer_draw(transform_t *transform, camera_t *camera) {
  VkResult result = VK_SUCCESS;

  result = vkWaitForFences(g_window.device, 1, &g_renderer.frame_fence, 1, UINT64_MAX);

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

  result = vkResetFences(g_window.device, 1, &g_renderer.frame_fence);

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

  result = vkAcquireNextImageKHR(g_window.device, g_swapchain.handle, UINT64_MAX, g_renderer.image_available_semaphore, 0, &g_renderer.image_index);

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
    .pWaitSemaphores = &g_renderer.image_available_semaphore,
    .waitSemaphoreCount = 1,
    .pSignalSemaphores = &g_renderer.render_finished_semaphore[g_renderer.image_index],
    .signalSemaphoreCount = 1,
    .pCommandBuffers = &g_window.command_buffer,
    .commandBufferCount = 1,
    .pWaitDstStageMask = graphics_wait_stages,
  };

  result = vkQueueSubmit(g_window.primary_queue, 1, &graphics_submit_info, g_renderer.frame_fence);

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
    .pWaitSemaphores = &g_renderer.render_finished_semaphore[g_renderer.image_index],
    .waitSemaphoreCount = 1,
    .pSwapchains = &g_swapchain.handle,
    .swapchainCount = 1,
    .pImageIndices = &g_renderer.image_index,
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

  renderer_destroy_pipeline();
  renderer_destroy_pipeline_layout();
  renderer_destroy_buffer();
  renderer_destroy_descriptor_set_layout();
  renderer_destroy_descriptor_pool();
  renderer_destroy_sync_object();
}

void renderer_draw_debug_line(vector3_t from, vector3_t to, vector4_t color) {
  if (g_renderer.is_debug_enabled) {

    uint32_t vertex_offset = g_renderer.debug_line_vertex_offset;
    uint32_t index_offset = g_renderer.debug_line_index_offset;

    if ((vertex_offset + 2) < DEBUG_LINE_VERTEX_COUNT &&
        (index_offset + 2) < DEBUG_LINE_INDEX_COUNT) {

      debug_line_vertex_t *vertices = (debug_line_vertex_t *)g_renderer.debug_line_vertex_buffer.mapped_memory;
      debug_line_index_t *indices = (debug_line_index_t *)g_renderer.debug_line_index_buffer.mapped_memory;

      vertices[vertex_offset + 0].position = from;
      vertices[vertex_offset + 1].position = to;

      vertices[vertex_offset + 0].color = color;
      vertices[vertex_offset + 1].color = color;

      indices[index_offset + 0] = (debug_line_index_t)(vertex_offset + 0);
      indices[index_offset + 1] = (debug_line_index_t)(vertex_offset + 1);

      g_renderer.debug_line_vertex_offset += 2;
      g_renderer.debug_line_index_offset += 2;
    }
  }
}
void renderer_draw_debug_box(vector3_t position, vector3_t size, vector4_t color) {
  if (g_renderer.is_debug_enabled) {

    uint32_t vertex_offset = g_renderer.debug_line_vertex_offset;
    uint32_t index_offset = g_renderer.debug_line_index_offset;

    if ((vertex_offset + 8) < DEBUG_LINE_VERTEX_COUNT &&
        (index_offset + 24) < DEBUG_LINE_INDEX_COUNT) {

      debug_line_vertex_t *vertices = (debug_line_vertex_t *)g_renderer.debug_line_vertex_buffer.mapped_memory;
      debug_line_index_t *indices = (debug_line_index_t *)g_renderer.debug_line_index_buffer.mapped_memory;

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

      g_renderer.debug_line_vertex_offset += 8;
      g_renderer.debug_line_index_offset += 24;
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

    VK_CHECK(vkCreateSemaphore(g_window.device, &semaphore_create_info, 0, &g_renderer.render_finished_semaphore[image_index]));

    image_index++;
  }

  VK_CHECK(vkCreateSemaphore(g_window.device, &semaphore_create_info, 0, &g_renderer.image_available_semaphore));
  VK_CHECK(vkCreateFence(g_window.device, &fence_create_info, 0, &g_renderer.frame_fence));
}
static void renderer_create_descriptor_pool(void) {
  {
    VkDescriptorPoolSize descriptor_pool_size[] = {
      {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
      },
      {
        .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        .descriptorCount = 1,
      },
    };

    VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
      .pPoolSizes = descriptor_pool_size,
      .poolSizeCount = ARRAY_COUNT(descriptor_pool_size),
      .maxSets = 1,
    };

    VK_CHECK(vkCreateDescriptorPool(g_window.device, &descriptor_pool_create_info, 0, &g_renderer.vdb_world_generator_descriptor_pool));
  }

  {
    VkDescriptorPoolSize descriptor_pool_size[] = {
      {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
      },
      {
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
      },
    };

    VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
      .pPoolSizes = descriptor_pool_size,
      .poolSizeCount = ARRAY_COUNT(descriptor_pool_size),
      .maxSets = 1,
    };

    VK_CHECK(vkCreateDescriptorPool(g_window.device, &descriptor_pool_create_info, 0, &g_renderer.vdb_geom_renderer_descriptor_pool));
  }

  {
    VkDescriptorPoolSize descriptor_pool_size[] = {
      {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
      },
    };

    VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
      .pPoolSizes = descriptor_pool_size,
      .poolSizeCount = ARRAY_COUNT(descriptor_pool_size),
      .maxSets = 1,
    };

    VK_CHECK(vkCreateDescriptorPool(g_window.device, &descriptor_pool_create_info, 0, &g_renderer.debug_line_descriptor_pool));
  }
}
static void renderer_create_descriptor_set_layout(void) {
  {
    VkDescriptorSetLayoutBinding descriptor_set_layout_binding[] = {
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
    };

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .pBindings = descriptor_set_layout_binding,
      .bindingCount = ARRAY_COUNT(descriptor_set_layout_binding),
      .pNext = 0,
    };

    VK_CHECK(vkCreateDescriptorSetLayout(g_window.device, &descriptor_set_layout_create_info, 0, &g_renderer.vdb_world_generator_descriptor_set_layout));
  }

  {
    VkDescriptorSetLayoutBinding descriptor_set_layout_binding[] = {
      {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_TASK_BIT_EXT | VK_SHADER_STAGE_MESH_BIT_EXT,
        .pImmutableSamplers = 0,
      },
      {
        .binding = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_MESH_BIT_EXT,
        .pImmutableSamplers = 0,
      },
    };

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .pBindings = descriptor_set_layout_binding,
      .bindingCount = ARRAY_COUNT(descriptor_set_layout_binding),
      .pNext = 0,
    };

    VK_CHECK(vkCreateDescriptorSetLayout(g_window.device, &descriptor_set_layout_create_info, 0, &g_renderer.vdb_geom_renderer_descriptor_set_layout));
  }

  {
    VkDescriptorSetLayoutBinding descriptor_set_layout_binding[] = {
      {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = 0,
      },
    };

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .pBindings = descriptor_set_layout_binding,
      .bindingCount = ARRAY_COUNT(descriptor_set_layout_binding),
      .pNext = 0,
    };

    VK_CHECK(vkCreateDescriptorSetLayout(g_window.device, &descriptor_set_layout_create_info, 0, &g_renderer.debug_line_descriptor_set_layout));
  }
}
static void renderer_create_descriptor_set(void) {
  {
    VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      .descriptorSetCount = 1,
      .descriptorPool = g_renderer.vdb_world_generator_descriptor_pool,
      .pSetLayouts = &g_renderer.vdb_world_generator_descriptor_set_layout,
    };

    VK_CHECK(vkAllocateDescriptorSets(g_window.device, &descriptor_set_allocate_info, &g_renderer.vdb_world_generator_descriptor_set));
  }

  {
    VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      .descriptorSetCount = 1,
      .descriptorPool = g_renderer.vdb_geom_renderer_descriptor_pool,
      .pSetLayouts = &g_renderer.vdb_geom_renderer_descriptor_set_layout,
    };

    VK_CHECK(vkAllocateDescriptorSets(g_window.device, &descriptor_set_allocate_info, &g_renderer.vdb_geom_renderer_descriptor_set));
  }

  {
    VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      .descriptorSetCount = 1,
      .descriptorPool = g_renderer.debug_line_descriptor_pool,
      .pSetLayouts = &g_renderer.debug_line_descriptor_set_layout,
    };

    VK_CHECK(vkAllocateDescriptorSets(g_window.device, &descriptor_set_allocate_info, &g_renderer.debug_line_descriptor_set));
  }
}
static void renderer_create_buffer(void) {
  full_screen_vertex_t full_screen_vertices[] = {
    {-1.0F, -1.0F, 0.0F},
    {1.0F, -1.0F, 0.0F},
    {-1.0F, 1.0F, 0.0F},
    {1.0F, 1.0F, 0.0F},
  };
  full_screen_index_t full_screen_indices[] = {
    0,
    1,
    2,
    3,
    2,
    1,
  };

  g_renderer.time_info_buffer = buffer_create_uniform_coherent(0, sizeof(time_info_t));
  g_renderer.screen_info_buffer = buffer_create_uniform_coherent(0, sizeof(screen_info_t));
  g_renderer.camera_info_buffer = buffer_create_uniform_coherent(0, sizeof(camera_info_t));

  g_renderer.debug_line_vertex_buffer = buffer_create_vertex_coherent(0, sizeof(debug_line_vertex_t) * DEBUG_LINE_VERTEX_COUNT);
  g_renderer.debug_line_index_buffer = buffer_create_index_coherent(0, sizeof(debug_line_index_t) * DEBUG_LINE_INDEX_COUNT);

  g_renderer.full_screen_vertex_buffer = buffer_create_vertex(full_screen_vertices, sizeof(full_screen_vertices));
  g_renderer.full_screen_index_buffer = buffer_create_index(full_screen_indices, sizeof(full_screen_indices));
}
static void renderer_create_pipeline_layout(void) {
  {
    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .setLayoutCount = 1,
      .pSetLayouts = &g_renderer.vdb_world_generator_descriptor_set_layout,
      .pPushConstantRanges = 0,
      .pushConstantRangeCount = 0,
    };

    VK_CHECK(vkCreatePipelineLayout(g_window.device, &pipeline_layout_create_info, 0, &g_renderer.vdb_world_generator_pipeline_layout));
  }

  {
    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .setLayoutCount = 1,
      .pSetLayouts = &g_renderer.vdb_geom_renderer_descriptor_set_layout,
      .pPushConstantRanges = 0,
      .pushConstantRangeCount = 0,
    };

    VK_CHECK(vkCreatePipelineLayout(g_window.device, &pipeline_layout_create_info, 0, &g_renderer.vdb_geom_renderer_pipeline_layout));
  }

  {
    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .setLayoutCount = 1,
      .pSetLayouts = &g_renderer.debug_line_descriptor_set_layout,
      .pPushConstantRanges = 0,
      .pushConstantRangeCount = 0,
    };

    VK_CHECK(vkCreatePipelineLayout(g_window.device, &pipeline_layout_create_info, 0, &g_renderer.debug_line_pipeline_layout));
  }
}

static void renderer_create_vdb_world_generator_pipeline(char const *compute_shader_file_path) {
  VkShaderModule compute_module = 0;

  {
    uint8_t *shader_bytes = 0;
    uint64_t shader_size = 0;

    fsutils_load_binary(&shader_bytes, &shader_size, compute_shader_file_path);

    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = (uint32_t const *)shader_bytes,
      .codeSize = shader_size,
    };

    VK_CHECK(vkCreateShaderModule(g_window.device, &shader_module_create_info, 0, &compute_module));

    HEAP_FREE(shader_bytes);
  }

  VkPipelineShaderStageCreateInfo compute_shader_stage_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    .stage = VK_SHADER_STAGE_COMPUTE_BIT,
    .module = compute_module,
    .pName = "main",
  };

  VkComputePipelineCreateInfo compute_pipeline_create_info = {
    .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
    .layout = g_renderer.vdb_world_generator_pipeline_layout,
    .stage = compute_shader_stage_create_info,
  };

  VK_CHECK(vkCreateComputePipelines(g_window.device, 0, 1, &compute_pipeline_create_info, 0, &g_renderer.vdb_world_generator_pipeline));

  vkDestroyShaderModule(g_window.device, compute_module, 0);
}
static void renderer_create_vdb_geom_renderer_pipeline(char const *task_shader_file_path, char const *mesh_shader_file_path, char const *fragment_shader_file_path) {
  VkShaderModule task_module = 0;
  VkShaderModule mesh_module = 0;
  VkShaderModule fragment_module = 0;

  {
    uint8_t *shader_bytes = 0;
    uint64_t shader_size = 0;

    fsutils_load_binary(&shader_bytes, &shader_size, task_shader_file_path);

    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = (uint32_t const *)shader_bytes,
      .codeSize = shader_size,
    };

    VK_CHECK(vkCreateShaderModule(g_window.device, &shader_module_create_info, 0, &task_module));

    HEAP_FREE(shader_bytes);
  }

  {
    uint8_t *shader_bytes = 0;
    uint64_t shader_size = 0;

    fsutils_load_binary(&shader_bytes, &shader_size, mesh_shader_file_path);

    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = (uint32_t const *)shader_bytes,
      .codeSize = shader_size,
    };

    VK_CHECK(vkCreateShaderModule(g_window.device, &shader_module_create_info, 0, &mesh_module));

    HEAP_FREE(shader_bytes);
  }

  {
    uint8_t *shader_bytes = 0;
    uint64_t shader_size = 0;

    fsutils_load_binary(&shader_bytes, &shader_size, fragment_shader_file_path);

    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = (uint32_t const *)shader_bytes,
      .codeSize = shader_size,
    };

    VK_CHECK(vkCreateShaderModule(g_window.device, &shader_module_create_info, 0, &fragment_module));

    HEAP_FREE(shader_bytes);
  }

  VkPipelineShaderStageCreateInfo task_shader_stage_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    .stage = VK_SHADER_STAGE_TASK_BIT_EXT,
    .module = task_module,
    .pName = "main",
  };
  VkPipelineShaderStageCreateInfo mesh_shader_stage_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    .stage = VK_SHADER_STAGE_MESH_BIT_EXT,
    .module = mesh_module,
    .pName = "main",
  };
  VkPipelineShaderStageCreateInfo fragment_shader_stage_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
    .module = fragment_module,
    .pName = "main",
  };

  VkPipelineShaderStageCreateInfo shader_stages[] = {
    task_shader_stage_create_info,
    mesh_shader_stage_create_info,
    fragment_shader_stage_create_info,
  };

  VkViewport viewport = {
    .x = 0.0F,
    .y = 0.0F,
    .width = (float)g_window.window_width,
    .height = (float)g_window.window_height,
    .minDepth = 0.0F,
    .maxDepth = 1.0F,
  };

  VkRect2D scissor = {
    .offset.x = 0,
    .offset.y = 0,
    .extent = {
      .width = g_window.window_width,
      .height = g_window.window_height,
    },
  };

  VkPipelineViewportStateCreateInfo viewport_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .viewportCount = 1,
    .pViewports = &viewport,
    .scissorCount = 1,
    .pScissors = &scissor,
  };

  VkPipelineRasterizationStateCreateInfo rasterizer_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    .depthClampEnable = 0,
    .rasterizerDiscardEnable = 0,
    .polygonMode = VK_POLYGON_MODE_FILL,
    .lineWidth = 1.0F,
    .cullMode = VK_CULL_MODE_BACK_BIT,
    .frontFace = VK_FRONT_FACE_CLOCKWISE,
    .depthBiasEnable = 0,
    .depthBiasConstantFactor = 0.0F,
    .depthBiasClamp = 0.0F,
    .depthBiasSlopeFactor = 0.0F,
  };

  VkPipelineMultisampleStateCreateInfo multisampling_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    .sampleShadingEnable = 0,
    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    .minSampleShading = 1.0F,
    .pSampleMask = 0,
    .alphaToCoverageEnable = 0,
    .alphaToOneEnable = 0,
  };

  VkPipelineColorBlendAttachmentState color_blend_attachment = {
    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    .blendEnable = 0,
    .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
    .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .colorBlendOp = VK_BLEND_OP_ADD,
    .srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .alphaBlendOp = VK_BLEND_OP_ADD,
  };

  VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
    .depthTestEnable = 1,
    .depthWriteEnable = 1,
    .depthCompareOp = VK_COMPARE_OP_LESS,
    .depthBoundsTestEnable = 0,
    .stencilTestEnable = 0,
  };

  VkPipelineColorBlendStateCreateInfo color_blend_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .logicOpEnable = 0,
    .logicOp = VK_LOGIC_OP_COPY,
    .attachmentCount = 1,
    .pAttachments = &color_blend_attachment,
    .blendConstants = {
      0.0F,
      0.0F,
      0.0F,
      0.0F,
    },
  };

  VkDynamicState dynamic_states[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR,
  };

  VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
    .pDynamicStates = dynamic_states,
    .dynamicStateCount = ARRAY_COUNT(dynamic_states),
  };

  VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .pStages = shader_stages,
    .stageCount = ARRAY_COUNT(shader_stages),
    .pVertexInputState = 0,
    .pInputAssemblyState = 0,
    .pViewportState = &viewport_state_create_info,
    .pRasterizationState = &rasterizer_create_info,
    .pMultisampleState = &multisampling_create_info,
    .pDepthStencilState = &depth_stencil_state_create_info,
    .pColorBlendState = &color_blend_create_info,
    .pDynamicState = &dynamic_state_create_info,
    .layout = g_renderer.vdb_geom_renderer_pipeline_layout,
    .renderPass = g_renderpass_main,
    .subpass = 0,
    .basePipelineHandle = 0,
  };

  VK_CHECK(vkCreateGraphicsPipelines(g_window.device, 0, 1, &graphics_pipeline_create_info, 0, &g_renderer.vdb_geom_renderer_pipeline));

  vkDestroyShaderModule(g_window.device, task_module, 0);
  vkDestroyShaderModule(g_window.device, mesh_module, 0);
  vkDestroyShaderModule(g_window.device, fragment_module, 0);
}
static void renderer_create_debug_line_pipeline(char const *vertex_shader_file_path, char const *fragment_shader_file_path) {
  VkShaderModule vertex_module = 0;
  VkShaderModule fragment_module = 0;

  {
    uint8_t *shader_bytes = 0;
    uint64_t shader_size = 0;

    fsutils_load_binary(&shader_bytes, &shader_size, vertex_shader_file_path);

    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = (uint32_t const *)shader_bytes,
      .codeSize = shader_size,
    };

    VK_CHECK(vkCreateShaderModule(g_window.device, &shader_module_create_info, 0, &vertex_module));

    HEAP_FREE(shader_bytes);
  }

  {
    uint8_t *shader_bytes = 0;
    uint64_t shader_size = 0;

    fsutils_load_binary(&shader_bytes, &shader_size, fragment_shader_file_path);

    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = (uint32_t const *)shader_bytes,
      .codeSize = shader_size,
    };

    VK_CHECK(vkCreateShaderModule(g_window.device, &shader_module_create_info, 0, &fragment_module));

    HEAP_FREE(shader_bytes);
  }

  VkPipelineShaderStageCreateInfo vertex_shader_stage_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    .stage = VK_SHADER_STAGE_VERTEX_BIT,
    .module = vertex_module,
    .pName = "main",
  };

  VkPipelineShaderStageCreateInfo fragment_shader_stage_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
    .module = fragment_module,
    .pName = "main",
  };

  VkPipelineShaderStageCreateInfo shader_stages[] = {
    vertex_shader_stage_create_info,
    fragment_shader_stage_create_info,
  };

  VkPipelineVertexInputStateCreateInfo vertex_input_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    .pVertexBindingDescriptions = s_debug_line_vertex_input_binding_description,
    .vertexBindingDescriptionCount = ARRAY_COUNT(s_debug_line_vertex_input_binding_description),
    .pVertexAttributeDescriptions = s_debug_line_vertex_input_attribute_description,
    .vertexAttributeDescriptionCount = ARRAY_COUNT(s_debug_line_vertex_input_attribute_description),
  };

  VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    .topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
    .primitiveRestartEnable = 0,
  };

  VkViewport viewport = {
    .x = 0.0F,
    .y = 0.0F,
    .width = (float)g_window.window_width,
    .height = (float)g_window.window_height,
    .minDepth = 0.0F,
    .maxDepth = 1.0F,
  };

  VkRect2D scissor = {
    .offset.x = 0,
    .offset.y = 0,
    .extent = {
      .width = g_window.window_width,
      .height = g_window.window_height,
    },
  };

  VkPipelineViewportStateCreateInfo viewport_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .viewportCount = 1,
    .pViewports = &viewport,
    .scissorCount = 1,
    .pScissors = &scissor,
  };

  VkPipelineRasterizationStateCreateInfo rasterizer_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    .depthClampEnable = 0,
    .rasterizerDiscardEnable = 0,
    .polygonMode = VK_POLYGON_MODE_FILL,
    .lineWidth = 1.0F,
    .cullMode = VK_CULL_MODE_BACK_BIT,
    .frontFace = VK_FRONT_FACE_CLOCKWISE,
    .depthBiasEnable = 0,
    .depthBiasConstantFactor = 0.0F,
    .depthBiasClamp = 0.0F,
    .depthBiasSlopeFactor = 0.0F,
  };

  VkPipelineMultisampleStateCreateInfo multisampling_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    .sampleShadingEnable = 0,
    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    .minSampleShading = 1.0F,
    .pSampleMask = 0,
    .alphaToCoverageEnable = 0,
    .alphaToOneEnable = 0,
  };

  VkPipelineColorBlendAttachmentState color_blend_attachment = {
    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    .blendEnable = 1,
    .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
    .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .colorBlendOp = VK_BLEND_OP_ADD,
    .srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .alphaBlendOp = VK_BLEND_OP_ADD,
  };

  VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
    .depthTestEnable = 0,
    .depthWriteEnable = 0,
    .depthCompareOp = VK_COMPARE_OP_LESS,
    .depthBoundsTestEnable = 0,
    .stencilTestEnable = 0,
  };

  VkPipelineColorBlendStateCreateInfo color_blend_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .logicOpEnable = 0,
    .logicOp = VK_LOGIC_OP_COPY,
    .attachmentCount = 1,
    .pAttachments = &color_blend_attachment,
    .blendConstants = {
      0.0F,
      0.0F,
      0.0F,
      0.0F,
    },
  };

  VkDynamicState dynamic_states[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR,
  };

  VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
    .pDynamicStates = dynamic_states,
    .dynamicStateCount = ARRAY_COUNT(dynamic_states),
  };

  VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .pStages = shader_stages,
    .stageCount = ARRAY_COUNT(shader_stages),
    .pVertexInputState = &vertex_input_create_info,
    .pInputAssemblyState = &input_assembly_create_info,
    .pViewportState = &viewport_state_create_info,
    .pRasterizationState = &rasterizer_create_info,
    .pMultisampleState = &multisampling_create_info,
    .pDepthStencilState = &depth_stencil_state_create_info,
    .pColorBlendState = &color_blend_create_info,
    .pDynamicState = &dynamic_state_create_info,
    .layout = g_renderer.debug_line_pipeline_layout,
    .renderPass = g_renderpass_main,
    .subpass = 0,
    .basePipelineHandle = 0,
  };

  VK_CHECK(vkCreateGraphicsPipelines(g_window.device, 0, 1, &graphics_pipeline_create_info, 0, &g_renderer.debug_line_pipeline));

  vkDestroyShaderModule(g_window.device, vertex_module, 0);
  vkDestroyShaderModule(g_window.device, fragment_module, 0);
}

static void renderer_update_vdb_world_generator_descriptor_set(void) {
  VkDescriptorBufferInfo vdb_cluster_info_descriptor_buffer_info[] = {
    {
      .offset = 0,
      .buffer = g_vdb.cluster_info_buffer.handle,
      .range = VK_WHOLE_SIZE,
    },
  };
  VkDescriptorImageInfo vdb_chunk_image_descriptor_image_info[] = {
    {
      .sampler = 0,
      .imageView = g_vdb.chunk_image_view,
      .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
    },
  };

  VkWriteDescriptorSet write_descriptor_set[] = {
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = g_renderer.vdb_world_generator_descriptor_set,
      .dstBinding = 0,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = ARRAY_COUNT(vdb_cluster_info_descriptor_buffer_info),
      .pImageInfo = 0,
      .pBufferInfo = vdb_cluster_info_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = g_renderer.vdb_world_generator_descriptor_set,
      .dstBinding = 1,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
      .descriptorCount = ARRAY_COUNT(vdb_chunk_image_descriptor_image_info),
      .pImageInfo = vdb_chunk_image_descriptor_image_info,
      .pBufferInfo = 0,
      .pTexelBufferView = 0,
    },
  };

  vkUpdateDescriptorSets(g_window.device, ARRAY_COUNT(write_descriptor_set), write_descriptor_set, 0, 0);
}
static void renderer_update_vdb_geom_renderer_descriptor_set(void) {
  VkDescriptorBufferInfo camera_info_descriptor_buffer_info[] = {
    {
      .offset = 0,
      .buffer = g_renderer.camera_info_buffer.handle,
      .range = VK_WHOLE_SIZE,
    },
  };
  VkDescriptorImageInfo vdb_chunk_image_descriptor_image_info[] = {
    {
      .sampler = g_vdb.chunk_sampler,
      .imageView = g_vdb.chunk_image_view,
      .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
    },
  };

  VkWriteDescriptorSet write_descriptor_set[] = {
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = g_renderer.vdb_geom_renderer_descriptor_set,
      .dstBinding = 0,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = ARRAY_COUNT(camera_info_descriptor_buffer_info),
      .pImageInfo = 0,
      .pBufferInfo = camera_info_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = g_renderer.vdb_geom_renderer_descriptor_set,
      .dstBinding = 1,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .descriptorCount = ARRAY_COUNT(vdb_chunk_image_descriptor_image_info),
      .pImageInfo = vdb_chunk_image_descriptor_image_info,
      .pBufferInfo = 0,
      .pTexelBufferView = 0,
    },
  };

  vkUpdateDescriptorSets(g_window.device, ARRAY_COUNT(write_descriptor_set), write_descriptor_set, 0, 0);
}
static void renderer_update_debug_line_descriptor_set(void) {
  VkDescriptorBufferInfo camera_info_descriptor_buffer_info[] = {
    {
      .offset = 0,
      .buffer = g_renderer.camera_info_buffer.handle,
      .range = VK_WHOLE_SIZE,
    },
  };

  VkWriteDescriptorSet write_descriptor_set[] = {
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = g_renderer.debug_line_descriptor_set,
      .dstBinding = 0,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = ARRAY_COUNT(camera_info_descriptor_buffer_info),
      .pImageInfo = 0,
      .pBufferInfo = camera_info_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
  };

  vkUpdateDescriptorSets(g_window.device, ARRAY_COUNT(write_descriptor_set), write_descriptor_set, 0, 0);
}

static void renderer_compute_world(void) {
  vkCmdBindPipeline(g_window.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, g_renderer.vdb_world_generator_pipeline);
  vkCmdBindDescriptorSets(g_window.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, g_renderer.vdb_world_generator_pipeline_layout, 0, 1, &g_renderer.vdb_world_generator_descriptor_set, 0, 0);

  int32_t group_count = MAKE_GROUP_COUNT(VDB_CHUNK_SIZE, 8);
  int32_t chunk_index = 0;
  int32_t chunk_count = VDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    vkCmdDispatch(g_window.command_buffer, group_count, group_count, group_count);

    VkBufferMemoryBarrier buffer_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
      .pNext = 0,
      .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .buffer = g_vdb.chunk_mask_buffer.handle,
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

    chunk_index++;
  }
}

static void renderer_update_uniform_buffer(transform_t *transform, camera_t *camera) {
  time_info_t *time_info = (time_info_t *)g_renderer.time_info_buffer.mapped_memory;
  time_info->time = g_window.time;
  time_info->delta_time = g_window.delta_time;

  screen_info_t *screen_info = (screen_info_t *)g_renderer.screen_info_buffer.mapped_memory;
  screen_info->resolution = vector2_xy((float)g_window.window_width, (float)g_window.window_height);

  camera_info_t *camera_info = (camera_info_t *)g_renderer.camera_info_buffer.mapped_memory;
  camera_info->position = transform->world_position;
  camera_info->view = camera->view;
  camera_info->projection = camera->projection;
  camera_info->view_projection = camera->view_projection;
  camera_info->view_projection_inv = camera->view_projection_inv;
}

static void renderer_record_compute_pass(void) {
  if (g_renderer.rebuild_world) {

    g_renderer.rebuild_world = 0;

    renderer_compute_world();
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
    .framebuffer = g_framebuffer_main.handle[g_renderer.image_index],
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
    int32_t group_count = MAKE_GROUP_COUNT(VDB_CHUNK_SIZE, 8);

    vkCmdBindPipeline(g_window.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_renderer.vdb_geom_renderer_pipeline);
    vkCmdBindDescriptorSets(g_window.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_renderer.vdb_geom_renderer_pipeline_layout, 0, 1, &g_renderer.vdb_geom_renderer_descriptor_set, 0, 0);
    vkCmdDrawMeshTasks(g_window.command_buffer, group_count, group_count, group_count);
  }

  {
    if (g_renderer.is_debug_enabled) {

      VkBuffer vertex_buffer[] = {g_renderer.debug_line_vertex_buffer.handle};
      VkDeviceSize vertex_offset[] = {0};

      vkCmdBindPipeline(g_window.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_renderer.debug_line_pipeline);
      vkCmdBindVertexBuffers(g_window.command_buffer, 0, ARRAY_COUNT(vertex_buffer), vertex_buffer, vertex_offset);
      vkCmdBindIndexBuffer(g_window.command_buffer, g_renderer.debug_line_index_buffer.handle, 0, VK_INDEX_TYPE_UINT32);
      vkCmdBindDescriptorSets(g_window.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_renderer.debug_line_pipeline_layout, 0, 1, &g_renderer.debug_line_descriptor_set, 0, 0);
      vkCmdDrawIndexed(g_window.command_buffer, g_renderer.debug_line_index_offset, 1, 0, 0, 0);

      g_renderer.debug_line_vertex_offset = 0;
      g_renderer.debug_line_index_offset = 0;
    }
  }

  dbgui_draw();

  vkCmdEndRenderPass(g_window.command_buffer);
}

static void renderer_destroy_sync_object(void) {
  int32_t image_index = 0;
  int32_t image_count = g_swapchain.image_count;

  while (image_index < image_count) {

    vkDestroySemaphore(g_window.device, g_renderer.render_finished_semaphore[image_index], 0);

    image_index++;
  }

  vkDestroySemaphore(g_window.device, g_renderer.image_available_semaphore, 0);
  vkDestroyFence(g_window.device, g_renderer.frame_fence, 0);
}
static void renderer_destroy_descriptor_pool(void) {
  vkDestroyDescriptorPool(g_window.device, g_renderer.vdb_world_generator_descriptor_pool, 0);
  vkDestroyDescriptorPool(g_window.device, g_renderer.vdb_geom_renderer_descriptor_pool, 0);
  vkDestroyDescriptorPool(g_window.device, g_renderer.debug_line_descriptor_pool, 0);
}
static void renderer_destroy_descriptor_set_layout(void) {
  vkDestroyDescriptorSetLayout(g_window.device, g_renderer.vdb_world_generator_descriptor_set_layout, 0);
  vkDestroyDescriptorSetLayout(g_window.device, g_renderer.vdb_geom_renderer_descriptor_set_layout, 0);
  vkDestroyDescriptorSetLayout(g_window.device, g_renderer.debug_line_descriptor_set_layout, 0);
}
static void renderer_destroy_buffer(void) {
  buffer_destroy(&g_renderer.time_info_buffer);
  buffer_destroy(&g_renderer.screen_info_buffer);
  buffer_destroy(&g_renderer.camera_info_buffer);

  buffer_destroy(&g_renderer.debug_line_vertex_buffer);
  buffer_destroy(&g_renderer.debug_line_index_buffer);

  buffer_destroy(&g_renderer.full_screen_vertex_buffer);
  buffer_destroy(&g_renderer.full_screen_index_buffer);
}
static void renderer_destroy_pipeline_layout(void) {
  vkDestroyPipelineLayout(g_window.device, g_renderer.vdb_world_generator_pipeline_layout, 0);
  vkDestroyPipelineLayout(g_window.device, g_renderer.vdb_geom_renderer_pipeline_layout, 0);
  vkDestroyPipelineLayout(g_window.device, g_renderer.debug_line_pipeline_layout, 0);
}
static void renderer_destroy_pipeline(void) {
  vkDestroyPipeline(g_window.device, g_renderer.vdb_world_generator_pipeline, 0);
  vkDestroyPipeline(g_window.device, g_renderer.vdb_geom_renderer_pipeline, 0);
  vkDestroyPipeline(g_window.device, g_renderer.debug_line_pipeline, 0);
}
