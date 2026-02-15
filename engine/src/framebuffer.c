#include <pch.h>

static void framebufffer_create_main_color_images(void);
static void framebufffer_create_main_depth_images(void);

framebuffer_t g_framebuffer_main = {0};

void framebuffer_create_main(void) {
  framebufffer_create_main_color_images();
  framebufffer_create_main_depth_images();

  uint32_t image_index = 0;
  uint32_t image_count = g_swapchain.image_count;

  while (image_index < image_count) {

    VkImageView image_attachments[] = {
      g_framebuffer_main.color_image_view[image_index],
      g_framebuffer_main.depth_image_view[image_index],
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

    VK_CHECK(vkCreateFramebuffer(g_window.device, &frame_buffer_create_info, 0, &g_framebuffer_main.handle[image_index]));

    image_index++;
  }
}

void framebuffer_destroy_main(void) {
  uint32_t image_index = 0;
  uint32_t image_count = g_swapchain.image_count;

  while (image_index < image_count) {

    vkDestroyFramebuffer(g_window.device, g_framebuffer_main.handle[image_index], 0);

    vkDestroyImageView(g_window.device, g_framebuffer_main.color_image_view[image_index], 0);

    vkDestroyImageView(g_window.device, g_framebuffer_main.depth_image_view[image_index], 0);
    vkFreeMemory(g_window.device, g_framebuffer_main.depth_device_memory[image_index], 0);
    vkDestroyImage(g_window.device, g_framebuffer_main.depth_image[image_index], 0);

    image_index++;
  }
}

static void framebufffer_create_main_color_images(void) {
  VK_CHECK(vkGetSwapchainImagesKHR(g_window.device, g_swapchain.handle, (uint32_t *)&g_swapchain.image_count, g_framebuffer_main.color_image));

  uint32_t image_index = 0;
  uint32_t image_count = g_swapchain.image_count;

  while (image_index < image_count) {

    VkImageViewCreateInfo image_view_create_info = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image = g_framebuffer_main.color_image[image_index],
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = g_window.prefered_surface_format.format,
      .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .subresourceRange.baseMipLevel = 0,
      .subresourceRange.levelCount = 1,
      .subresourceRange.baseArrayLayer = 0,
      .subresourceRange.layerCount = 1,
    };

    VK_CHECK(vkCreateImageView(g_window.device, &image_view_create_info, 0, &g_framebuffer_main.color_image_view[image_index]));

    image_index++;
  }
}
static void framebufffer_create_main_depth_images(void) {
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

    VK_CHECK(vkCreateImage(g_window.device, &image_create_info, 0, &g_framebuffer_main.depth_image[image_index]));

    VkMemoryRequirements memory_requirements = {0};

    vkGetImageMemoryRequirements(g_window.device, g_framebuffer_main.depth_image[image_index], &memory_requirements);

    uint32_t memory_type_index = vkutils_find_memory_type_index(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkMemoryAllocateInfo memory_allocate_info = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize = memory_requirements.size,
      .memoryTypeIndex = memory_type_index,
    };

    VK_CHECK(vkAllocateMemory(g_window.device, &memory_allocate_info, 0, &g_framebuffer_main.depth_device_memory[image_index]));
    VK_CHECK(vkBindImageMemory(g_window.device, g_framebuffer_main.depth_image[image_index], g_framebuffer_main.depth_device_memory[image_index], 0));

    VkImageViewCreateInfo image_view_create_info = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image = g_framebuffer_main.depth_image[image_index],
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = VK_FORMAT_D32_SFLOAT,
      .subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
      .subresourceRange.baseMipLevel = 0,
      .subresourceRange.levelCount = 1,
      .subresourceRange.baseArrayLayer = 0,
      .subresourceRange.layerCount = 1,
    };

    VK_CHECK(vkCreateImageView(g_window.device, &image_view_create_info, 0, &g_framebuffer_main.depth_image_view[image_index]));

    image_index++;
  }
}
