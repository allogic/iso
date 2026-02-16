#include <pch.h>

void image_create(image_t *image) {
  image->image_usage_flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  image->memory_property_flags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

  VkImageCreateInfo image_create_info = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    .imageType = image->image_type,
    .extent.width = image->width,
    .extent.height = image->height,
    .extent.depth = image->depth,
    .mipLevels = 1,
    .arrayLayers = 1,
    .format = image->format,
    .tiling = image->image_tiling,
    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    .usage = image->image_usage_flags,
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
  };

  VK_CHECK(vkCreateImage(g_window.device, &image_create_info, 0, &image->image_handle));

  VkMemoryRequirements memory_requirements = {0};

  vkGetImageMemoryRequirements(g_window.device, image->image_handle, &memory_requirements);

  uint32_t memory_type_index = vkutils_find_memory_type_index(memory_requirements.memoryTypeBits, image->memory_property_flags);

  VkMemoryAllocateInfo memory_allocate_info = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
    .allocationSize = memory_requirements.size,
    .memoryTypeIndex = memory_type_index,
  };

  VK_CHECK(vkAllocateMemory(g_window.device, &memory_allocate_info, 0, &image->device_memory));
  VK_CHECK(vkBindImageMemory(g_window.device, image->image_handle, image->device_memory, 0));

  VkImageViewCreateInfo image_view_create_info = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    .image = image->image_handle,
    .viewType = image->image_view_type,
    .format = image->format,
    .subresourceRange.aspectMask = image->image_aspect_flags,
    .subresourceRange.baseMipLevel = 0,
    .subresourceRange.levelCount = 1,
    .subresourceRange.baseArrayLayer = 0,
    .subresourceRange.layerCount = 1,
  };

  VK_CHECK(vkCreateImageView(g_window.device, &image_view_create_info, 0, &image->image_view));

  VkSamplerCreateInfo sampler_create_info = {
    .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
    .magFilter = image->filter,
    .minFilter = image->filter,
    .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    .anisotropyEnable = 0,
    .maxAnisotropy = 0.0F,
    .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
    .unnormalizedCoordinates = 0,
    .compareEnable = 0,
    .compareOp = VK_COMPARE_OP_ALWAYS,
    .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
    .mipLodBias = 0.0F,
    .minLod = 0.0F,
    .maxLod = VK_LOD_CLAMP_NONE,
  };

  VK_CHECK(vkCreateSampler(g_window.device, &sampler_create_info, 0, &image->sampler));

  buffer_t staging_buffer = {
    .host_data = image->host_data,
    .size = image->element_size * image->width * image->height * image->depth * image->channel,
    .buffer_usage_flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    .memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
  };

  {
    VkBufferCreateInfo buffer_create_info = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size = staging_buffer.size,
      .usage = staging_buffer.buffer_usage_flags,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    VK_CHECK(vkCreateBuffer(g_window.device, &buffer_create_info, 0, &staging_buffer.buffer_handle));

    VkMemoryRequirements memory_requirements = {0};

    vkGetBufferMemoryRequirements(g_window.device, staging_buffer.buffer_handle, &memory_requirements);

    uint32_t memory_type_index = vkutils_find_memory_type_index(memory_requirements.memoryTypeBits, staging_buffer.memory_property_flags);

    VkMemoryAllocateInfo memory_allocate_info = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize = memory_requirements.size,
      .memoryTypeIndex = memory_type_index,
    };

    VK_CHECK(vkAllocateMemory(g_window.device, &memory_allocate_info, 0, &staging_buffer.device_memory));
    VK_CHECK(vkBindBufferMemory(g_window.device, staging_buffer.buffer_handle, staging_buffer.device_memory, 0));
  }

  if (staging_buffer.host_data) {

    VK_CHECK(vkMapMemory(g_window.device, staging_buffer.device_memory, 0, staging_buffer.size, 0, &staging_buffer.device_data));

    memcpy(staging_buffer.device_data, staging_buffer.host_data, staging_buffer.size);

    vkUnmapMemory(g_window.device, staging_buffer.device_memory);

    staging_buffer.device_data = 0;
  }

  VkCommandBufferBeginInfo command_buffer_begin_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
  };

  VK_CHECK(vkBeginCommandBuffer(g_window.command_buffer, &command_buffer_begin_info));

  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = image->image_handle,
      .subresourceRange = {
        .aspectMask = image->image_aspect_flags,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
      .srcAccessMask = VK_ACCESS_NONE,
      .dstAccessMask = VK_ACCESS_NONE,
    };

    vkCmdPipelineBarrier(
      g_window.command_buffer,
      VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
      VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
      0,
      0,
      0,
      0,
      0,
      1,
      &image_memory_barrier);
  }

  VkBufferImageCopy buffer_image_copy = {
    .bufferOffset = 0,
    .bufferRowLength = 0,
    .bufferImageHeight = 0,
    .imageSubresource = {
      .aspectMask = image->image_aspect_flags,
      .mipLevel = 0,
      .baseArrayLayer = 0,
      .layerCount = 1,
    },
    .imageOffset = {
      .x = 0,
      .y = 0,
      .z = 0,
    },
    .imageExtent = {
      .width = image->width,
      .height = image->height,
      .depth = image->depth,
    },
  };

  vkCmdCopyBufferToImage(g_window.command_buffer, staging_buffer.buffer_handle, image->image_handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &buffer_image_copy);

  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_GENERAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = image->image_handle,
      .subresourceRange = {
        .aspectMask = image->image_aspect_flags,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
      .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_NONE,
    };

    vkCmdPipelineBarrier(
      g_window.command_buffer,
      VK_PIPELINE_STAGE_TRANSFER_BIT,
      VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
      0,
      0,
      0,
      0,
      0,
      1,
      &image_memory_barrier);
  }

  VK_CHECK(vkEndCommandBuffer(g_window.command_buffer));

  VkSubmitInfo submit_info = {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .commandBufferCount = 1,
    .pCommandBuffers = &g_window.command_buffer,
  };

  VK_CHECK(vkQueueSubmit(g_window.primary_queue, 1, &submit_info, 0));
  VK_CHECK(vkQueueWaitIdle(g_window.primary_queue));

  vkFreeMemory(g_window.device, staging_buffer.device_memory, 0);
  vkDestroyBuffer(g_window.device, staging_buffer.buffer_handle, 0);
}
void image_map(image_t *image) {
  VK_CHECK(vkMapMemory(g_window.device, image->device_memory, 0, image->element_size * image->width * image->height * image->depth * image->channel, 0, &image->device_data));
}
void image_unmap(image_t *image) {
  vkUnmapMemory(g_window.device, image->device_memory);

  image->device_data = 0;
}
void image_destroy(image_t *image) {
  if (image->device_data) {

    vkUnmapMemory(g_window.device, image->device_memory);

    image->device_data = 0;
  }

  vkFreeMemory(g_window.device, image->device_memory, 0);

  vkDestroySampler(g_window.device, image->sampler, 0);
  vkDestroyImageView(g_window.device, image->image_view, 0);
  vkDestroyImage(g_window.device, image->image_handle, 0);
}
