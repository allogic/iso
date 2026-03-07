#include <pch.h>

#define DEBUG_LINE_VERTEX_COUNT (0xFFFFF)
#define DEBUG_LINE_INDEX_COUNT (0xFFFFF)

static void renderer_create_sync_object(void);
static void renderer_create_command_pool(void);
static void renderer_create_command_buffer(void);
static void renderer_create_coherent_buffer(void);
static void renderer_create_debug_line_buffer(void);
static void renderer_create_full_screen_buffer(void);

static void renderer_update_debug_line_descriptor_set(void);

static void renderer_update_coherent_buffer(void);

static void renderer_record_compute_pass(void);
static void renderer_record_main_pass(void);
static void renderer_record_ray_tracing_pass(void);

static void renderer_destroy_sync_object(void);
static void renderer_destroy_command_pool(void);
static void renderer_destroy_command_buffer(void);
static void renderer_destroy_buffer(void);

static uint32_t s_debug_line_vertex_offset = 0;
static uint32_t s_debug_line_index_offset = 0;

static VkFence s_frame_fence = 0;

static VkSemaphore s_render_finished_semaphore[SWAPCHAIN_MAX_IMAGE_COUNT] = {0};
static VkSemaphore s_image_available_semaphore = {0};

static full_screen_vertex_t s_full_screen_vertices[] = {
  {-1.0F, -1.0F, 0.0F, 1.0F},
  {1.0F, -1.0F, 0.0F, 1.0F},
  {-1.0F, 1.0F, 0.0F, 1.0F},
  {1.0F, 1.0F, 0.0F, 1.0F},
};
static full_screen_index_t s_full_screen_indices[] = {
  0,
  1,
  2,
  3,
  2,
  1,
};

static VkVertexInputBindingDescription s_full_screen_vertex_input_binding_description[] = {
  {
    .binding = 0,
    .stride = sizeof(full_screen_vertex_t),
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
  },
};
static VkVertexInputBindingDescription s_debug_line_vertex_input_binding_description[] = {
  {
    .binding = 0,
    .stride = sizeof(debug_line_vertex_t),
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
  },
};

static VkVertexInputAttributeDescription s_full_screen_vertex_input_attribute_description[] = {
  {
    .location = 0,
    .binding = 0,
    .format = VK_FORMAT_R32G32B32A32_SFLOAT,
    .offset = 0,
  },
};
static VkVertexInputAttributeDescription s_debug_line_vertex_input_attribute_description[] = {
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
    .offset = OFFSET_OF(debug_line_vertex_t, color),
  },
};

static VkDescriptorPoolSize s_debug_line_renderer_descriptor_pool_size[] = {
  {
    .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
  },
};

static VkDescriptorSetLayoutBinding s_debug_line_renderer_descriptor_set_layout_binding[] = {
  {
    .binding = 0,
    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    .pImmutableSamplers = 0,
  },
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
  .host_data = s_full_screen_vertices,
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
static buffer_t s_mouse_info_buffer = {
  .size = sizeof(mouse_info_t),
  .buffer_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
};
static buffer_t s_camera_info_buffer = {
  .size = sizeof(camera_info_t),
  .buffer_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
};

static pipeline_t s_debug_line_renderer_pipeline = {
  .pipeline_type = PIPELINE_TYPE_DFLT,
  .vertex_shader = ROOT_DIR "/shader/debug/line_renderer.vert.spv",
  .fragment_shader = ROOT_DIR "/shader/debug/line_renderer.frag.spv",
  .primitive_topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
  .polygon_mode = VK_POLYGON_MODE_FILL,
  .cull_mode = VK_CULL_MODE_BACK_BIT,
  .enable_blending = 1,
  .enable_depth_test = 1,
  .enable_depth_write = 1,
  .vertex_input_binding_description = s_debug_line_vertex_input_binding_description,
  .vertex_input_binding_description_count = ARRAY_COUNT(s_debug_line_vertex_input_binding_description),
  .vertex_input_attribute_description = s_debug_line_vertex_input_attribute_description,
  .vertex_input_attribute_description_count = ARRAY_COUNT(s_debug_line_vertex_input_attribute_description),
  .descriptor_pool_size = s_debug_line_renderer_descriptor_pool_size,
  .descriptor_pool_size_count = ARRAY_COUNT(s_debug_line_renderer_descriptor_pool_size),
  .descriptor_set_layout_binding = s_debug_line_renderer_descriptor_set_layout_binding,
  .descriptor_set_layout_binding_count = ARRAY_COUNT(s_debug_line_renderer_descriptor_set_layout_binding),
  .descriptor_set_count = 1,
  .render_pass = &g_renderpass,
};

renderer_t g_renderer = {0};

void renderer_create(void) {
  g_renderer.is_debug_enabled = 1;

  renderer_create_sync_object();
  renderer_create_command_pool();
  renderer_create_command_buffer();
  renderer_create_coherent_buffer();
  renderer_create_debug_line_buffer();
  renderer_create_full_screen_buffer();

  pipeline_create(&s_debug_line_renderer_pipeline);

  renderer_update_debug_line_descriptor_set();
}
void renderer_draw(void) {
  VK_CHECK(vkWaitForFences(g_window.device, 1, &s_frame_fence, 1, UINT64_MAX));
  VK_CHECK(vkResetFences(g_window.device, 1, &s_frame_fence));

  if (g_chunkmgr.async_state == CHUNKMGR_ASYNC_STATE_IN_FLIGHT) {

    if (g_chunkmgr.build_state == CHUNKMGR_BUILD_STATE_DIRTY) {
      g_chunkmgr.build_state = CHUNKMGR_BUILD_STATE_READY;
    }

    // g_chunkmgr.async_state = CHUNKMGR_ASYNC_STATE_IDLE;
    g_chunkmgr.async_state = CHUNKMGR_ASYNC_STATE_COUNT;
  }

  VK_CHECK(vkAcquireNextImageKHR(g_window.device, g_swapchain.handle, UINT64_MAX, s_image_available_semaphore, 0, &g_renderer.image_index));

  renderer_update_coherent_buffer();

  VkCommandBufferBeginInfo command_buffer_begin_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    .pInheritanceInfo = 0,
  };

  VK_CHECK(vkResetCommandBuffer(g_renderer.command_buffer, 0));
  VK_CHECK(vkBeginCommandBuffer(g_renderer.command_buffer, &command_buffer_begin_info));

  if (g_chunkmgr.async_state == CHUNKMGR_ASYNC_STATE_READY) {

    if (g_chunkmgr.build_state == CHUNKMGR_BUILD_STATE_DIRTY) {
      vkCmdExecuteCommands(g_renderer.command_buffer, 1, &g_chunkmgr.command_buffer);
    }

    g_chunkmgr.async_state = CHUNKMGR_ASYNC_STATE_IN_FLIGHT;
  }

  renderer_record_compute_pass();
  renderer_record_main_pass();
  renderer_record_ray_tracing_pass();

  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = g_framebuffer.color_image[g_renderer.image_index],
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
      .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
    };

    vkCmdPipelineBarrier(g_renderer.command_buffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }

  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = g_swapchain.image[g_renderer.image_index],
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
      .srcAccessMask = VK_ACCESS_NONE,
      .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
    };

    vkCmdPipelineBarrier(g_renderer.command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }

  VkImageCopy image_copy = {
    .srcSubresource = {
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .baseArrayLayer = 0,
      .layerCount = 1,
      .mipLevel = 0,
    },
    .srcOffset = {
      .x = 0,
      .y = 0,
      .z = 0,
    },
    .dstSubresource = {
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .baseArrayLayer = 0,
      .layerCount = 1,
      .mipLevel = 0,
    },
    .dstOffset = {
      .x = 0,
      .y = 0,
      .z = 0,
    },
    .extent = {
      .width = g_window.window_width,
      .height = g_window.window_height,
      .depth = 1,
    },
  };

  vkCmdCopyImage(g_renderer.command_buffer, g_framebuffer.color_image[g_renderer.image_index], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, g_swapchain.image[g_renderer.image_index], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &image_copy);

  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = g_framebuffer.color_image[g_renderer.image_index],
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
      .srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
      .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    };

    vkCmdPipelineBarrier(g_renderer.command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }

  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = g_swapchain.image[g_renderer.image_index],
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
      .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_NONE,
    };

    vkCmdPipelineBarrier(g_renderer.command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }

  VK_CHECK(vkEndCommandBuffer(g_renderer.command_buffer));

  VkPipelineStageFlags primary_wait_stages[] = {
    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR,
    VK_PIPELINE_STAGE_TRANSFER_BIT,
  };

  VkSubmitInfo primary_submit_info = {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .pWaitSemaphores = &s_image_available_semaphore,
    .waitSemaphoreCount = 1,
    .pSignalSemaphores = &s_render_finished_semaphore[g_renderer.image_index],
    .signalSemaphoreCount = 1,
    .pCommandBuffers = &g_renderer.command_buffer,
    .commandBufferCount = 1,
    .pWaitDstStageMask = primary_wait_stages,
  };

  VK_CHECK(vkQueueSubmit(g_window.primary_queue, 1, &primary_submit_info, s_frame_fence));

  VkPresentInfoKHR present_info = {
    .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
    .pWaitSemaphores = &s_render_finished_semaphore[g_renderer.image_index],
    .waitSemaphoreCount = 1,
    .pSwapchains = &g_swapchain.handle,
    .swapchainCount = 1,
    .pImageIndices = &g_renderer.image_index,
  };

  VkResult result = vkQueuePresentKHR(g_window.present_queue, &present_info);

  switch (result) {
    case VK_SUBOPTIMAL_KHR:
    case VK_ERROR_OUT_OF_DATE_KHR: {

      g_swapchain.is_dirty = 1;

      return;
    }
  }
}
void renderer_destroy(void) {
  pipeline_destroy(&s_debug_line_renderer_pipeline);

  renderer_destroy_buffer();
  renderer_destroy_command_buffer();
  renderer_destroy_command_pool();
  renderer_destroy_sync_object();
}

void renderer_draw_debug_line(vector3_t from, vector3_t to, vector4_t color) {
  if (g_renderer.is_debug_enabled) {

    uint32_t vertex_offset = s_debug_line_vertex_offset;
    uint32_t index_offset = s_debug_line_index_offset;

    if ((vertex_offset + 2) < DEBUG_LINE_VERTEX_COUNT &&
        (index_offset + 2) < DEBUG_LINE_INDEX_COUNT) {

      debug_line_vertex_t *vertices = (debug_line_vertex_t *)s_debug_line_vertex_buffer.device_data;
      debug_line_index_t *indices = (debug_line_index_t *)s_debug_line_index_buffer.device_data;

      vertices[vertex_offset + 0].position = (vector4_t){from.x, from.y, from.z, 1.0F};
      vertices[vertex_offset + 1].position = (vector4_t){to.x, to.y, to.z, 1.0F};

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

      vertices[vertex_offset + 0].position = (vector4_t){position.x, position.y, position.z, 1.0F};
      vertices[vertex_offset + 1].position = (vector4_t){position.x, position.y + size.y, position.z, 1.00F};
      vertices[vertex_offset + 2].position = (vector4_t){position.x + size.x, position.y, position.z, 1.0F};
      vertices[vertex_offset + 3].position = (vector4_t){position.x + size.x, position.y + size.y, position.z, 1.0F};
      vertices[vertex_offset + 4].position = (vector4_t){position.x, position.y, position.z + size.z, 1.0F};
      vertices[vertex_offset + 5].position = (vector4_t){position.x, position.y + size.y, position.z + size.z, 1.0F};
      vertices[vertex_offset + 6].position = (vector4_t){position.x + size.x, position.y, position.z + size.z, 1.0F};
      vertices[vertex_offset + 7].position = (vector4_t){position.x + size.x, position.y + size.y, position.z + size.z, 1.0F};

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

  uint32_t image_index = 0;
  uint32_t image_count = g_swapchain.image_count;

  while (image_index < image_count) {

    VK_CHECK(vkCreateSemaphore(g_window.device, &semaphore_create_info, 0, &s_render_finished_semaphore[image_index]));

    image_index++;
  }

  VK_CHECK(vkCreateSemaphore(g_window.device, &semaphore_create_info, 0, &s_image_available_semaphore));
  VK_CHECK(vkCreateFence(g_window.device, &fence_create_info, 0, &s_frame_fence));
}
static void renderer_create_command_pool(void) {
  VkCommandPoolCreateInfo command_pool_create_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    .queueFamilyIndex = g_window.primary_queue_index,
  };

  VK_CHECK(vkCreateCommandPool(g_window.device, &command_pool_create_info, 0, &g_renderer.command_pool));
}
static void renderer_create_command_buffer(void) {
  VkCommandBufferAllocateInfo command_buffer_allocate_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .commandPool = g_renderer.command_pool,
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = 1,
  };

  VK_CHECK(vkAllocateCommandBuffers(g_window.device, &command_buffer_allocate_info, &g_renderer.command_buffer));
}
static void renderer_create_coherent_buffer(void) {
  buffer_create(&s_time_info_buffer);
  buffer_create(&s_screen_info_buffer);
  buffer_create(&s_mouse_info_buffer);
  buffer_create(&s_camera_info_buffer);

  buffer_map(&s_time_info_buffer);
  buffer_map(&s_screen_info_buffer);
  buffer_map(&s_mouse_info_buffer);
  buffer_map(&s_camera_info_buffer);

  g_renderer.time_info = (time_info_t *)s_time_info_buffer.device_data;
  g_renderer.screen_info = (screen_info_t *)s_screen_info_buffer.device_data;
  g_renderer.mouse_info = (mouse_info_t *)s_mouse_info_buffer.device_data;
  g_renderer.camera_info = (camera_info_t *)s_camera_info_buffer.device_data;

  g_renderer.time_info_descriptor_buffer_info.offset = 0;
  g_renderer.time_info_descriptor_buffer_info.buffer = s_time_info_buffer.buffer_handle;
  g_renderer.time_info_descriptor_buffer_info.range = VK_WHOLE_SIZE;

  g_renderer.screen_info_descriptor_buffer_info.offset = 0;
  g_renderer.screen_info_descriptor_buffer_info.buffer = s_screen_info_buffer.buffer_handle;
  g_renderer.screen_info_descriptor_buffer_info.range = VK_WHOLE_SIZE;

  g_renderer.mouse_info_descriptor_buffer_info.offset = 0;
  g_renderer.mouse_info_descriptor_buffer_info.buffer = s_mouse_info_buffer.buffer_handle;
  g_renderer.mouse_info_descriptor_buffer_info.range = VK_WHOLE_SIZE;

  g_renderer.camera_info_descriptor_buffer_info.offset = 0;
  g_renderer.camera_info_descriptor_buffer_info.buffer = s_camera_info_buffer.buffer_handle;
  g_renderer.camera_info_descriptor_buffer_info.range = VK_WHOLE_SIZE;
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

static void renderer_update_debug_line_descriptor_set(void) {
  VkWriteDescriptorSet write_descriptor_set[] = {
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = s_debug_line_renderer_pipeline.descriptor_set[0],
      .dstBinding = 0,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = 1,
      .pImageInfo = 0,
      .pBufferInfo = &g_renderer.camera_info_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
  };

  vkUpdateDescriptorSets(g_window.device, ARRAY_COUNT(write_descriptor_set), write_descriptor_set, 0, 0);
}

static void renderer_update_coherent_buffer(void) {
  g_renderer.time_info->time = g_window.time;
  g_renderer.time_info->delta_time = g_window.delta_time;

  g_renderer.screen_info->resolution = (ivector2_t){g_window.window_width, g_window.window_height};

  g_renderer.mouse_info->position = (ivector2_t){g_window.mouse_position_x, g_window.mouse_position_y};

  vector3_t camera_position = g_player.transform.world_position;
  vector3_t camera_direction = quaternion_front(g_player.transform.world_rotation);

  g_renderer.camera_info->position = (vector4_t){camera_position.x, camera_position.y, camera_position.z, 0.0F};
  g_renderer.camera_info->direction = (vector4_t){camera_direction.x, camera_direction.y, camera_direction.z, 0.0F};
  g_renderer.camera_info->view = g_player.camera.view;
  g_renderer.camera_info->view_inv = g_player.camera.view_inv;
  g_renderer.camera_info->projection = g_player.camera.projection;
  g_renderer.camera_info->projection_inv = g_player.camera.projection_inv;
  g_renderer.camera_info->view_projection = g_player.camera.view_projection;
  g_renderer.camera_info->view_projection_inv = g_player.camera.view_projection_inv;
}

static void renderer_record_compute_pass(void) {
  // if (g_svdb.generate_world) {
  //
  //   g_svdb.generate_world = 0;
  //   g_svdb.is_dirty = 1;
  //
  //   // uint32_t chunk_index = 0;
  //   // uint32_t chunk_count = SVDB_CHUNK_COUNT;
  //   //
  //   // while (chunk_index < chunk_count) {
  //   //
  //   //   svdb_generate_world(g_renderer.command_buffer, chunk_index);
  //   //   svdb_generate_mask(g_renderer.command_buffer, chunk_index);
  //   //   svdb_generate_mesh(g_renderer.command_buffer, chunk_index);
  //   //
  //   //   chunk_index++;
  //   // }
  // }

  // if (g_svdb.rebuild_chunk) {
  //
  //   g_svdb.rebuild_chunk = 0;
  //   g_svdb.is_dirty = 1;
  //
  //   // TODO: handle rebuild depending for surrounding chunks..
  //   // svdb_generate_mask(g_renderer.command_buffer, 0);
  //   // svdb_generate_mesh(g_renderer.command_buffer, 0);
  // }

  // TODO: handle selection that go outside the current chunk..
  // svdb_select_voxel();

  // if (g_player.place_voxel) {
  //
  //   g_player.place_voxel = 0;
  //
  //   svdb_place_voxel();
  //
  //   g_svdb.rebuild_chunk = 1;
  // }
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
    .renderPass = g_renderpass,
    .framebuffer = g_framebuffer.handle[g_renderer.image_index],
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

  vkCmdBeginRenderPass(g_renderer.command_buffer, &render_pass_create_info, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport = {
    .x = 0.0F,
    .y = 0.0F,
    .width = (float)g_window.window_width,
    .height = (float)g_window.window_height,
    .minDepth = 0.0F,
    .maxDepth = 1.0F,
  };

  vkCmdSetViewport(g_renderer.command_buffer, 0, 1, &viewport);

  VkRect2D scissor = {
    .offset.x = 0,
    .offset.y = 0,
    .extent = {
      .width = g_window.window_width,
      .height = g_window.window_height,
    },
  };

  vkCmdSetScissor(g_renderer.command_buffer, 0, 1, &scissor);

  svdb_draw_mesh(g_renderer.command_buffer);

  if (g_renderer.is_debug_enabled) {

    VkDeviceSize vertex_offset = 0;

    vkCmdBindPipeline(g_renderer.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, s_debug_line_renderer_pipeline.pipeline_handle);
    vkCmdBindVertexBuffers(g_renderer.command_buffer, 0, 1, &s_debug_line_vertex_buffer.buffer_handle, &vertex_offset);
    vkCmdBindIndexBuffer(g_renderer.command_buffer, s_debug_line_index_buffer.buffer_handle, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(g_renderer.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, s_debug_line_renderer_pipeline.pipeline_layout, 0, 1, &s_debug_line_renderer_pipeline.descriptor_set[0], 0, 0);
    vkCmdDrawIndexed(g_renderer.command_buffer, s_debug_line_index_offset, 1, 0, 0, 0);

    s_debug_line_vertex_offset = 0;
    s_debug_line_index_offset = 0;
  }

  dbgui_draw(g_renderer.command_buffer);

  vkCmdEndRenderPass(g_renderer.command_buffer);
}
static void renderer_record_ray_tracing_pass(void) {
  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_GENERAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = g_framebuffer.color_image[g_renderer.image_index],
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
      .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
    };

    vkCmdPipelineBarrier(g_renderer.command_buffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }

  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = g_framebuffer.depth_image[g_renderer.image_index],
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
      .srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
    };

    vkCmdPipelineBarrier(g_renderer.command_buffer, VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }

  // dvdb_draw(g_renderer.command_buffer);

  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
      .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = g_framebuffer.color_image[g_renderer.image_index],
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
      .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    };

    vkCmdPipelineBarrier(g_renderer.command_buffer, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }

  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = g_framebuffer.depth_image[g_renderer.image_index],
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
      .srcAccessMask = VK_ACCESS_SHADER_READ_BIT,
      .dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
    };

    vkCmdPipelineBarrier(g_renderer.command_buffer, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }
}

static void renderer_destroy_sync_object(void) {
  uint32_t image_index = 0;
  uint32_t image_count = g_swapchain.image_count;

  while (image_index < image_count) {

    vkDestroySemaphore(g_window.device, s_render_finished_semaphore[image_index], 0);

    image_index++;
  }

  vkDestroySemaphore(g_window.device, s_image_available_semaphore, 0);
  vkDestroyFence(g_window.device, s_frame_fence, 0);
}
static void renderer_destroy_command_pool(void) {
  vkDestroyCommandPool(g_window.device, g_renderer.command_pool, 0);
}
static void renderer_destroy_command_buffer(void) {
  vkFreeCommandBuffers(g_window.device, g_renderer.command_pool, 1, &g_renderer.command_buffer);
}
static void renderer_destroy_buffer(void) {
  buffer_destroy(&s_debug_line_vertex_buffer);
  buffer_destroy(&s_debug_line_index_buffer);
  buffer_destroy(&s_full_screen_vertex_buffer);
  buffer_destroy(&s_full_screen_index_buffer);
  buffer_destroy(&s_time_info_buffer);
  buffer_destroy(&s_screen_info_buffer);
  buffer_destroy(&s_mouse_info_buffer);
  buffer_destroy(&s_camera_info_buffer);
}
