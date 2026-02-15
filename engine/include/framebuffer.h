#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

typedef struct framebuffer_t {
  VkImage color_image[SWAPCHAIN_MAX_IMAGE_COUNT];
  VkImageView color_image_view[SWAPCHAIN_MAX_IMAGE_COUNT];
  VkImage depth_image[SWAPCHAIN_MAX_IMAGE_COUNT];
  VkDeviceMemory depth_device_memory[SWAPCHAIN_MAX_IMAGE_COUNT];
  VkImageView depth_image_view[SWAPCHAIN_MAX_IMAGE_COUNT];
  VkFramebuffer handle[SWAPCHAIN_MAX_IMAGE_COUNT];
} framebuffer_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern framebuffer_t g_framebuffer_main;

void framebuffer_create_main(void);

void framebuffer_destroy_main(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // FRAMEBUFFER_H
