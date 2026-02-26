#include <pch.h>

static void framebuffer_create_main_images(void);
static void framebuffer_create_depth_images(void);
static void framebuffer_create_ray_images(void);

framebuffer_t g_framebuffer = {0};

void framebuffer_create(void) {
  framebuffer_create_main_images();
  framebuffer_create_depth_images();
  framebuffer_create_ray_images();

  uint32_t image_index = 0;
  uint32_t image_count = g_swapchain.image_count;

  while (image_index < image_count) {

    VkImageView image_attachments[] = {
      g_framebuffer.main_image_view[image_index],
      g_framebuffer.depth_image_view[image_index],
    };

    VkFramebufferCreateInfo frame_buffer_create_info = {
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .renderPass = g_renderpass_main,
      .pAttachments = image_attachments,
      .attachmentCount = ARRAY_COUNT(image_attachments),
      .width = g_window.window_width,
      .height = g_window.window_height,
      .layers = 1,
    };

    VK_CHECK(vkCreateFramebuffer(g_window.device, &frame_buffer_create_info, 0, &g_framebuffer.handle[image_index]));

    image_index++;
  }
}
void framebuffer_destroy(void) {
  uint32_t image_index = 0;
  uint32_t image_count = g_swapchain.image_count;

  while (image_index < image_count) {

    vkDestroyFramebuffer(g_window.device, g_framebuffer.handle[image_index], 0);

    vkDestroyImageView(g_window.device, g_framebuffer.main_image_view[image_index], 0);

    vkDestroyImageView(g_window.device, g_framebuffer.depth_image_view[image_index], 0);
    vkFreeMemory(g_window.device, g_framebuffer.depth_device_memory[image_index], 0);
    vkDestroyImage(g_window.device, g_framebuffer.depth_image[image_index], 0);

    vkDestroyImageView(g_window.device, g_framebuffer.ray_image_view[image_index], 0);
    vkFreeMemory(g_window.device, g_framebuffer.ray_device_memory[image_index], 0);
    vkDestroyImage(g_window.device, g_framebuffer.ray_image[image_index], 0);

    image_index++;
  }
}

static void framebuffer_create_main_images(void) {
  VK_CHECK(vkGetSwapchainImagesKHR(g_window.device, g_swapchain.handle, (uint32_t *)&g_swapchain.image_count, g_framebuffer.main_image));

  uint32_t image_index = 0;
  uint32_t image_count = g_swapchain.image_count;

  while (image_index < image_count) {

    VkImageViewCreateInfo image_view_create_info = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image = g_framebuffer.main_image[image_index],
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = g_window.prefered_surface_format.format,
      .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .subresourceRange.baseMipLevel = 0,
      .subresourceRange.levelCount = 1,
      .subresourceRange.baseArrayLayer = 0,
      .subresourceRange.layerCount = 1,
    };

    VK_CHECK(vkCreateImageView(g_window.device, &image_view_create_info, 0, &g_framebuffer.main_image_view[image_index]));

    image_index++;
  }
}
static void framebuffer_create_depth_images(void) {
  uint32_t image_index = 0;
  uint32_t image_count = g_swapchain.image_count;

  while (image_index < image_count) {

    VkImageCreateInfo image_create_info = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .imageType = VK_IMAGE_TYPE_2D,
      .extent.width = g_window.window_width,
      .extent.height = g_window.window_height,
      .extent.depth = 1,
      .mipLevels = 1,
      .arrayLayers = 1,
      .format = VK_FORMAT_D32_SFLOAT,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    VK_CHECK(vkCreateImage(g_window.device, &image_create_info, 0, &g_framebuffer.depth_image[image_index]));

    VkMemoryRequirements memory_requirements = {0};

    vkGetImageMemoryRequirements(g_window.device, g_framebuffer.depth_image[image_index], &memory_requirements);

    uint32_t memory_type_index = vkutil_find_memory_type_index(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkMemoryAllocateInfo memory_allocate_info = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize = memory_requirements.size,
      .memoryTypeIndex = memory_type_index,
    };

    VK_CHECK(vkAllocateMemory(g_window.device, &memory_allocate_info, 0, &g_framebuffer.depth_device_memory[image_index]));
    VK_CHECK(vkBindImageMemory(g_window.device, g_framebuffer.depth_image[image_index], g_framebuffer.depth_device_memory[image_index], 0));

    VkImageViewCreateInfo image_view_create_info = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image = g_framebuffer.depth_image[image_index],
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = VK_FORMAT_D32_SFLOAT,
      .subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
      .subresourceRange.baseMipLevel = 0,
      .subresourceRange.levelCount = 1,
      .subresourceRange.baseArrayLayer = 0,
      .subresourceRange.layerCount = 1,
    };

    VK_CHECK(vkCreateImageView(g_window.device, &image_view_create_info, 0, &g_framebuffer.depth_image_view[image_index]));

    image_index++;
  }
}
static void framebuffer_create_ray_images(void) {
  uint32_t image_index = 0;
  uint32_t image_count = g_swapchain.image_count;

  while (image_index < image_count) {

    VkImageCreateInfo image_create_info = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .imageType = VK_IMAGE_TYPE_2D,
      .extent.width = g_window.window_width,
      .extent.height = g_window.window_height,
      .extent.depth = 1,
      .mipLevels = 1,
      .arrayLayers = 1,
      .format = VK_FORMAT_R8G8B8A8_UNORM,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .usage = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    VK_CHECK(vkCreateImage(g_window.device, &image_create_info, 0, &g_framebuffer.ray_image[image_index]));

    VkMemoryRequirements memory_requirements = {0};

    vkGetImageMemoryRequirements(g_window.device, g_framebuffer.ray_image[image_index], &memory_requirements);

    uint32_t memory_type_index = vkutil_find_memory_type_index(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkMemoryAllocateInfo memory_allocate_info = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize = memory_requirements.size,
      .memoryTypeIndex = memory_type_index,
    };

    VK_CHECK(vkAllocateMemory(g_window.device, &memory_allocate_info, 0, &g_framebuffer.ray_device_memory[image_index]));
    VK_CHECK(vkBindImageMemory(g_window.device, g_framebuffer.ray_image[image_index], g_framebuffer.ray_device_memory[image_index], 0));

    VkImageViewCreateInfo image_view_create_info = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image = g_framebuffer.ray_image[image_index],
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = VK_FORMAT_R8G8B8A8_UNORM,
      .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .subresourceRange.baseMipLevel = 0,
      .subresourceRange.levelCount = 1,
      .subresourceRange.baseArrayLayer = 0,
      .subresourceRange.layerCount = 1,
    };

    VK_CHECK(vkCreateImageView(g_window.device, &image_view_create_info, 0, &g_framebuffer.ray_image_view[image_index]));

    VkCommandBufferBeginInfo command_buffer_begin_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    VK_CHECK(vkBeginCommandBuffer(g_window.command_buffer, &command_buffer_begin_info));

    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .newLayout = VK_IMAGE_LAYOUT_GENERAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = g_framebuffer.ray_image[image_index],
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
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

    VK_CHECK(vkEndCommandBuffer(g_window.command_buffer));

    VkSubmitInfo submit_info = {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .commandBufferCount = 1,
      .pCommandBuffers = &g_window.command_buffer,
    };

    VK_CHECK(vkQueueSubmit(g_window.primary_queue, 1, &submit_info, 0));
    VK_CHECK(vkQueueWaitIdle(g_window.primary_queue));

    image_index++;
  }
}
