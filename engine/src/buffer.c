#include <pch.h>

buffer_t buffer_create(uint64_t size, VkBufferUsageFlags buffer_usage_flags, VkMemoryPropertyFlags memory_property_flags) {
  buffer_t buffer = {
    .size = size,
  };

  VkBufferCreateInfo buffer_create_info = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    .size = size,
    .usage = buffer_usage_flags,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
  };

  VK_CHECK(vkCreateBuffer(g_window.device, &buffer_create_info, 0, &buffer.handle));

  VkMemoryRequirements memory_requirements = {0};

  vkGetBufferMemoryRequirements(g_window.device, buffer.handle, &memory_requirements);

  uint32_t memory_type_index = vkutils_find_memory_type_index(memory_requirements.memoryTypeBits, memory_property_flags);

  VkMemoryAllocateInfo memory_allocate_info = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
    .allocationSize = memory_requirements.size,
    .memoryTypeIndex = memory_type_index,
  };

  VK_CHECK(vkAllocateMemory(g_window.device, &memory_allocate_info, 0, &buffer.device_memory));
  VK_CHECK(vkBindBufferMemory(g_window.device, buffer.handle, buffer.device_memory, 0));

  return buffer;
}
void buffer_map(buffer_t *buffer) {
  VK_CHECK(vkMapMemory(g_window.device, buffer->device_memory, 0, buffer->size, 0, &buffer->mapped_memory));
}
void buffer_unmap(buffer_t *buffer) {
  vkUnmapMemory(g_window.device, buffer->device_memory);

  buffer->mapped_memory = 0;
}
void buffer_copy(buffer_t *src_buffer, buffer_t *dst_buffer, uint64_t src_offset, uint64_t dst_offset, uint64_t size) {
  VkCommandBufferBeginInfo command_buffer_begin_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
  };

  VK_CHECK(vkBeginCommandBuffer(g_window.command_buffer, &command_buffer_begin_info));

  VkBufferCopy buffer_copy = {
    .srcOffset = src_offset,
    .dstOffset = dst_offset,
    .size = size,
  };

  vkCmdCopyBuffer(g_window.command_buffer, src_buffer->handle, dst_buffer->handle, 1, &buffer_copy);

  VK_CHECK(vkEndCommandBuffer(g_window.command_buffer));

  VkSubmitInfo submit_info = {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .commandBufferCount = 1,
    .pCommandBuffers = &g_window.command_buffer,
  };

  VK_CHECK(vkQueueSubmit(g_window.primary_queue, 1, &submit_info, 0));
  VK_CHECK(vkQueueWaitIdle(g_window.primary_queue));
}
void buffer_destroy(buffer_t *buffer) {
  if (buffer->mapped_memory) {

    vkUnmapMemory(g_window.device, buffer->device_memory);

    buffer->mapped_memory = 0;
  }

  vkFreeMemory(g_window.device, buffer->device_memory, 0);
  vkDestroyBuffer(g_window.device, buffer->handle, 0);
}

buffer_t buffer_create_vertex(void *data, uint64_t size) {
  buffer_t staging_buffer = buffer_create(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  buffer_t target_buffer = buffer_create(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  if (data) {
    buffer_map(&staging_buffer);
    memcpy(staging_buffer.mapped_memory, data, size);
    buffer_unmap(&staging_buffer);
  }

  buffer_copy(&staging_buffer, &target_buffer, 0, 0, size);
  buffer_destroy(&staging_buffer);

  return target_buffer;
}
buffer_t buffer_create_index(void *data, uint64_t size) {
  buffer_t staging_buffer = buffer_create(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  buffer_t target_buffer = buffer_create(size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  if (data) {
    buffer_map(&staging_buffer);
    memcpy(staging_buffer.mapped_memory, data, size);
    buffer_unmap(&staging_buffer);
  }

  buffer_copy(&staging_buffer, &target_buffer, 0, 0, size);
  buffer_destroy(&staging_buffer);

  return target_buffer;
}
buffer_t buffer_create_uniform(void *data, uint64_t size) {
  buffer_t staging_buffer = buffer_create(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  buffer_t target_buffer = buffer_create(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  if (data) {
    buffer_map(&staging_buffer);
    memcpy(staging_buffer.mapped_memory, data, size);
    buffer_unmap(&staging_buffer);
  }

  buffer_copy(&staging_buffer, &target_buffer, 0, 0, size);
  buffer_destroy(&staging_buffer);

  return target_buffer;
}
buffer_t buffer_create_storage(void *data, uint64_t size) {
  buffer_t staging_buffer = buffer_create(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  buffer_t target_buffer = buffer_create(size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  if (data) {
    buffer_map(&staging_buffer);
    memcpy(staging_buffer.mapped_memory, data, size);
    buffer_unmap(&staging_buffer);
  }

  buffer_copy(&staging_buffer, &target_buffer, 0, 0, size);
  buffer_destroy(&staging_buffer);

  return target_buffer;
}

buffer_t buffer_create_vertex_coherent(void *data, uint64_t size) {
  buffer_t target_buffer = buffer_create(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  buffer_map(&target_buffer);

  if (data) {
    memcpy(target_buffer.mapped_memory, data, size);
  }

  return target_buffer;
}
buffer_t buffer_create_index_coherent(void *data, uint64_t size) {
  buffer_t target_buffer = buffer_create(size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  buffer_map(&target_buffer);

  if (data) {
    memcpy(target_buffer.mapped_memory, data, size);
  }

  return target_buffer;
}
buffer_t buffer_create_uniform_coherent(void *data, uint64_t size) {
  buffer_t target_buffer = buffer_create(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  buffer_map(&target_buffer);

  if (data) {
    memcpy(target_buffer.mapped_memory, data, size);
  }

  return target_buffer;
}
buffer_t buffer_create_storage_coherent(void *data, uint64_t size) {
  buffer_t target_buffer = buffer_create(size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  buffer_map(&target_buffer);

  if (data) {
    memcpy(target_buffer.mapped_memory, data, size);
  }

  return target_buffer;
}
