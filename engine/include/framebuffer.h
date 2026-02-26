#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

typedef struct framebuffer_t {
  VkImage main_image[SWAPCHAIN_MAX_IMAGE_COUNT];
  VkImageView main_image_view[SWAPCHAIN_MAX_IMAGE_COUNT];
  VkImage depth_image[SWAPCHAIN_MAX_IMAGE_COUNT];
  VkDeviceMemory depth_device_memory[SWAPCHAIN_MAX_IMAGE_COUNT];
  VkImageView depth_image_view[SWAPCHAIN_MAX_IMAGE_COUNT];
  VkImage ray_image[SWAPCHAIN_MAX_IMAGE_COUNT];
  VkDeviceMemory ray_device_memory[SWAPCHAIN_MAX_IMAGE_COUNT];
  VkImageView ray_image_view[SWAPCHAIN_MAX_IMAGE_COUNT];
  VkFramebuffer handle[SWAPCHAIN_MAX_IMAGE_COUNT];
} framebuffer_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern framebuffer_t g_framebuffer;

void framebuffer_create(void);
void framebuffer_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // FRAMEBUFFER_H
