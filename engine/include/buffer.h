#ifndef BUFFER_H
#define BUFFER_H

typedef struct buffer_t {
  uint64_t size;
  VkBuffer handle;
  VkDeviceMemory device_memory;
  void *mapped_memory;
} buffer_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

buffer_t buffer_create(uint64_t size, VkBufferUsageFlags buffer_usage_flags, VkMemoryPropertyFlags memory_property_flags);
void buffer_map(buffer_t *buffer);
void buffer_unmap(buffer_t *buffer);
void buffer_copy(buffer_t *src_buffer, buffer_t *dst_buffer, uint64_t src_offset, uint64_t dst_offset, uint64_t size);
void buffer_destroy(buffer_t *buffer);

buffer_t buffer_create_vertex(void *data, uint64_t size);
buffer_t buffer_create_index(void *data, uint64_t size);
buffer_t buffer_create_uniform(void *data, uint64_t size);
buffer_t buffer_create_storage(void *data, uint64_t size);

buffer_t buffer_create_vertex_coherent(void *data, uint64_t size);
buffer_t buffer_create_index_coherent(void *data, uint64_t size);
buffer_t buffer_create_uniform_coherent(void *data, uint64_t size);
buffer_t buffer_create_storage_coherent(void *data, uint64_t size);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // BUFFER_H
