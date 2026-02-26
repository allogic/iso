#include <pch.h>

swapchain_t g_swapchain = {0};

void swapchain_create(void) {
  g_swapchain.image_count = clampu(SWAPCHAIN_IMAGE_COUNT, g_window.min_image_count, g_window.max_image_count);

  uint32_t queue_families[2] = {
    g_window.primary_queue_index,
    g_window.present_queue_index,
  };

  VkSwapchainCreateInfoKHR swapchain_create_info = {
    .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    .surface = g_window.surface,
    .minImageCount = g_swapchain.image_count,
    .imageFormat = VK_FORMAT_R8G8B8A8_UNORM,
    .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
    .imageExtent.width = g_window.window_width,
    .imageExtent.height = g_window.window_height,
    .imageArrayLayers = 1,
    .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
    .preTransform = g_window.surface_transform,
    .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    .presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
    .clipped = 1,
    .oldSwapchain = 0,
    .imageSharingMode = VK_SHARING_MODE_CONCURRENT,
    .pQueueFamilyIndices = queue_families,
    .queueFamilyIndexCount = ARRAY_COUNT(queue_families),
  };

  VK_CHECK(vkCreateSwapchainKHR(g_window.device, &swapchain_create_info, 0, &g_swapchain.handle));
  VK_CHECK(vkGetSwapchainImagesKHR(g_window.device, g_swapchain.handle, &g_swapchain.image_count, g_swapchain.image));
}
void swapchain_destroy(void) {
  vkDestroySwapchainKHR(g_window.device, g_swapchain.handle, 0);
}
