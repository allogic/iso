#ifndef IMAGE_H
#define IMAGE_H

typedef struct image_t {
  void *host_data;
  void *device_data;
  uint32_t width;
  uint32_t height;
  uint32_t depth;
  uint32_t channel;
  uint64_t element_size;
  VkFormat format;
  VkFilter filter;
  VkImageUsageFlags image_usage_flags;
  VkMemoryPropertyFlags memory_property_flags;
  VkImageType image_type;
  VkImageViewType image_view_type;
  VkImageAspectFlags image_aspect_flags;
  VkImageTiling image_tiling;
  VkImage image_handle;
  VkImageView image_view;
  VkDeviceMemory device_memory;
  VkSampler sampler;
} image_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void image_create(image_t *image);
void image_map(image_t *image);
void image_unmap(image_t *image);
void image_destroy(image_t *image);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // IMAGE_H
