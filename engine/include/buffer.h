#ifndef BUFFER_H
#define BUFFER_H

typedef struct buffer_t {
  uint64_t size;
  VkBufferUsageFlags usage;
  VkMemoryPropertyFlags memory_properties;
  VkMemoryRequirements memory_requirements;
  VkBuffer handle;
  VkDeviceMemory device_memory;
  void *mapped_memory;
} buffer_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

buffer_t buffer_create(uint64_t buffer_size, VkBufferUsageFlags buffer_usage_flags, VkMemoryPropertyFlags memory_properties);
void buffer_destroy(buffer_t *buffer);

buffer_t buffer_create_vertex(void *buffer, uint64_t buffer_size);
buffer_t buffer_create_index(void *buffer, uint64_t buffer_size);
buffer_t buffer_create_uniform(void *buffer, uint64_t buffer_size);
buffer_t buffer_create_storage(void *buffer, uint64_t buffer_size);

buffer_t buffer_create_vertex_coherent(void *buffer, uint64_t buffer_size);
buffer_t buffer_create_index_coherent(void *buffer, uint64_t buffer_size);
buffer_t buffer_create_uniform_coherent(void *buffer, uint64_t buffer_size);
buffer_t buffer_create_storage_coherent(void *buffer, uint64_t buffer_size);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // BUFFER_H
