#include <pch.h>

swapchain_t g_swapchain = {0};

void swapchain_create(int32_t image_count) {
  g_swapchain.image_count = clampi(image_count, g_window.min_image_count, g_window.max_image_count);

  int32_t queue_families[2] = {
    g_window.primary_queue_index,
    g_window.present_queue_index,
  };

  VkSwapchainCreateInfoKHR swapchain_create_info = {
    .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    .surface = g_window.surface,
    .minImageCount = g_swapchain.image_count,
    .imageFormat = g_window.prefered_surface_format.format,
    .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
    .imageExtent.width = g_window.window_width,
    .imageExtent.height = g_window.window_height,
    .imageArrayLayers = 1,
    .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    .preTransform = g_window.surface_transform,
    .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    .presentMode = g_window.prefered_present_mode,
    .clipped = 1,
    .oldSwapchain = 0,
    .imageSharingMode = VK_SHARING_MODE_CONCURRENT,
    .pQueueFamilyIndices = queue_families,
    .queueFamilyIndexCount = ARRAY_COUNT(queue_families),
  };

  VK_CHECK(vkCreateSwapchainKHR(g_window.device, &swapchain_create_info, 0, &g_swapchain.handle));
}
void swapchain_destroy(void) {
  vkDestroySwapchainKHR(g_window.device, g_swapchain.handle, 0);
}
