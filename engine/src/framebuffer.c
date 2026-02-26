#include <pch.h>

static void framebuffer_create_color_images(void);
static void framebuffer_create_depth_images(void);

framebuffer_t g_framebuffer = {0};

void framebuffer_create(void) {
  framebuffer_create_color_images();
  framebuffer_create_depth_images();

  uint32_t image_index = 0;
  uint32_t image_count = g_swapchain.image_count;

  while (image_index < image_count) {

    VkImageView image_attachments[] = {
      g_framebuffer.color_image_view[image_index],
      g_framebuffer.depth_image_view[image_index],
    };

    VkFramebufferCreateInfo frame_buffer_create_info = {
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .renderPass = g_renderpass,
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

    vkDestroySampler(g_window.device, g_framebuffer.color_sampler[image_index], 0);
    vkDestroyImageView(g_window.device, g_framebuffer.color_image_view[image_index], 0);
    vkFreeMemory(g_window.device, g_framebuffer.color_device_memory[image_index], 0);
    vkDestroyImage(g_window.device, g_framebuffer.color_image[image_index], 0);

    vkDestroySampler(g_window.device, g_framebuffer.depth_sampler[image_index], 0);
    vkDestroyImageView(g_window.device, g_framebuffer.depth_image_view[image_index], 0);
    vkFreeMemory(g_window.device, g_framebuffer.depth_device_memory[image_index], 0);
    vkDestroyImage(g_window.device, g_framebuffer.depth_image[image_index], 0);

    image_index++;
  }
}

static void framebuffer_create_color_images(void) {
  uint32_t image_index = 0;
  uint32_t image_count = g_swapchain.image_count;

  while (image_index < image_count) {

    VkImageCreateInfo image_create_info = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .imageType = VK_IMAGE_TYPE_2D,
      .extent = {
        .width = g_window.window_width,
        .height = g_window.window_height,
        .depth = 1,
      },
      .mipLevels = 1,
      .arrayLayers = 1,
      .format = VK_FORMAT_R8G8B8A8_UNORM,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    VK_CHECK(vkCreateImage(g_window.device, &image_create_info, 0, &g_framebuffer.color_image[image_index]));

    VkMemoryRequirements memory_requirements = {0};

    vkGetImageMemoryRequirements(g_window.device, g_framebuffer.color_image[image_index], &memory_requirements);

    uint32_t memory_type_index = vkutil_find_memory_type_index(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkMemoryAllocateInfo memory_allocate_info = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize = memory_requirements.size,
      .memoryTypeIndex = memory_type_index,
    };

    VK_CHECK(vkAllocateMemory(g_window.device, &memory_allocate_info, 0, &g_framebuffer.color_device_memory[image_index]));
    VK_CHECK(vkBindImageMemory(g_window.device, g_framebuffer.color_image[image_index], g_framebuffer.color_device_memory[image_index], 0));

    VkImageViewCreateInfo image_view_create_info = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image = g_framebuffer.color_image[image_index],
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = VK_FORMAT_R8G8B8A8_UNORM,
      .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .subresourceRange.baseMipLevel = 0,
      .subresourceRange.levelCount = 1,
      .subresourceRange.baseArrayLayer = 0,
      .subresourceRange.layerCount = 1,
    };

    VK_CHECK(vkCreateImageView(g_window.device, &image_view_create_info, 0, &g_framebuffer.color_image_view[image_index]));

    VkSamplerCreateInfo sampler_create_info = {
      .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
      .magFilter = VK_FILTER_NEAREST,
      .minFilter = VK_FILTER_NEAREST,
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

    VK_CHECK(vkCreateSampler(g_window.device, &sampler_create_info, 0, &g_framebuffer.color_sampler[image_index]));

    // TODO: move this into the renderer!
    g_framebuffer.color_descriptor_image_info[image_index].sampler = g_framebuffer.color_sampler[image_index],
    g_framebuffer.color_descriptor_image_info[image_index].imageView = g_framebuffer.color_image_view[image_index],
    g_framebuffer.color_descriptor_image_info[image_index].imageLayout = VK_IMAGE_LAYOUT_GENERAL,

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
      .extent = {
        .width = g_window.window_width,
        .height = g_window.window_height,
        .depth = 1,
      },
      .mipLevels = 1,
      .arrayLayers = 1,
      .format = VK_FORMAT_D32_SFLOAT,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
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

    VkSamplerCreateInfo sampler_create_info = {
      .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
      .magFilter = VK_FILTER_NEAREST,
      .minFilter = VK_FILTER_NEAREST,
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

    VK_CHECK(vkCreateSampler(g_window.device, &sampler_create_info, 0, &g_framebuffer.depth_sampler[image_index]));

    // TODO: move this into the renderer!
    g_framebuffer.depth_descriptor_image_info[image_index].sampler = g_framebuffer.depth_sampler[image_index],
    g_framebuffer.depth_descriptor_image_info[image_index].imageView = g_framebuffer.depth_image_view[image_index],
    g_framebuffer.depth_descriptor_image_info[image_index].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,

    image_index++;
  }
}
