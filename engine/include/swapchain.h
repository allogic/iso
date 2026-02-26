#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#define SWAPCHAIN_IMAGE_COUNT (3)
#define SWAPCHAIN_MAX_IMAGE_COUNT (0x10)

typedef struct swapchain_t {
  uint32_t is_dirty;
  uint32_t image_count;
  VkImage image[SWAPCHAIN_MAX_IMAGE_COUNT];
  VkSwapchainKHR handle;
} swapchain_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern swapchain_t g_swapchain;

void swapchain_create(void);
void swapchain_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SWAPCHAIN_H
