#ifndef BUFFER_H
#define BUFFER_H

typedef struct buffer_t {
  void *host_data;
  void *device_data;
  uint64_t size;
  VkBufferUsageFlags buffer_usage_flags;
  VkMemoryPropertyFlags memory_property_flags;
  VkBuffer buffer_handle;
  VkDeviceMemory device_memory;
} buffer_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void buffer_create(buffer_t *buffer);
void buffer_map(buffer_t *buffer);
void buffer_unmap(buffer_t *buffer);
void buffer_destroy(buffer_t *buffer);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // BUFFER_H
