#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#define SWAPCHAIN_MAX_IMAGE_COUNT (0x10)

typedef struct swapchain_t {
  int8_t is_dirty;
  int32_t image_count;
  VkSwapchainKHR handle;
} swapchain_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern swapchain_t g_swapchain;

void swapchain_create(int32_t image_count);
void swapchain_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SWAPCHAIN_H
