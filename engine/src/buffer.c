#include <pch.h>

buffer_t buffer_create(uint64_t buffer_size, VkBufferUsageFlags usage_flags, VkMemoryPropertyFlags memory_properties) {
  buffer_t buffer = {
    .size = buffer_size,
    .usage = usage_flags,
    .memory_properties = memory_properties,
  };

  VkBufferCreateInfo buffer_create_info = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    .size = buffer.size,
    .usage = buffer.usage,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
  };

  VK_CHECK(vkCreateBuffer(g_window.device, &buffer_create_info, 0, &buffer.handle));

  vkGetBufferMemoryRequirements(g_window.device, buffer.handle, &buffer.memory_requirements);

  uint32_t memory_type_index = vkutils_find_memory_type_index(buffer.memory_requirements.memoryTypeBits, buffer.memory_properties);

  VkMemoryAllocateInfo memory_allocate_info = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
    .allocationSize = buffer.memory_requirements.size,
    .memoryTypeIndex = memory_type_index,
  };

  VK_CHECK(vkAllocateMemory(g_window.device, &memory_allocate_info, 0, &buffer.device_memory));
  VK_CHECK(vkBindBufferMemory(g_window.device, buffer.handle, buffer.device_memory, 0));

  return buffer;
}
void buffer_destroy(buffer_t *buffer) {
  if (buffer->mapped_memory) {

    vkUnmapMemory(g_window.device, buffer->device_memory);

    buffer->mapped_memory = 0;
  }

  vkFreeMemory(g_window.device, buffer->device_memory, 0);
  vkDestroyBuffer(g_window.device, buffer->handle, 0);
}

buffer_t buffer_create_vertex(void *buffer, uint64_t buffer_size) {
  buffer_t staging_buffer = buffer_create(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  buffer_t target_buffer = buffer_create(buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  if (buffer) {

    VK_CHECK(vkMapMemory(g_window.device, staging_buffer.device_memory, 0, staging_buffer.size, 0, &staging_buffer.mapped_memory));

    memcpy(staging_buffer.mapped_memory, buffer, buffer_size);

    vkUnmapMemory(g_window.device, staging_buffer.device_memory);

    staging_buffer.mapped_memory = 0;
  }

  VkCommandBuffer command_buffer = vkutils_begin_command_buffer();

  VkBufferCopy buffer_copy = {
    .srcOffset = 0,
    .dstOffset = 0,
    .size = buffer_size,
  };

  vkCmdCopyBuffer(command_buffer, staging_buffer.handle, target_buffer.handle, 1, &buffer_copy);

  vkutils_end_command_buffer(command_buffer);

  buffer_destroy(&staging_buffer);

  return target_buffer;
}
buffer_t buffer_create_index(void *buffer, uint64_t buffer_size) {
  buffer_t staging_buffer = buffer_create(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  buffer_t target_buffer = buffer_create(buffer_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  if (buffer) {

    VK_CHECK(vkMapMemory(g_window.device, staging_buffer.device_memory, 0, staging_buffer.size, 0, &staging_buffer.mapped_memory));

    memcpy(staging_buffer.mapped_memory, buffer, buffer_size);

    vkUnmapMemory(g_window.device, staging_buffer.device_memory);

    staging_buffer.mapped_memory = 0;
  }

  VkCommandBuffer command_buffer = vkutils_begin_command_buffer();

  VkBufferCopy buffer_copy = {
    .srcOffset = 0,
    .dstOffset = 0,
    .size = buffer_size,
  };

  vkCmdCopyBuffer(command_buffer, staging_buffer.handle, target_buffer.handle, 1, &buffer_copy);

  vkutils_end_command_buffer(command_buffer);

  buffer_destroy(&staging_buffer);

  return target_buffer;
}
buffer_t buffer_create_uniform(void *buffer, uint64_t buffer_size) {
  buffer_t staging_buffer = buffer_create(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  buffer_t target_buffer = buffer_create(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  if (buffer) {

    VK_CHECK(vkMapMemory(g_window.device, staging_buffer.device_memory, 0, staging_buffer.size, 0, &staging_buffer.mapped_memory));

    memcpy(staging_buffer.mapped_memory, buffer, buffer_size);

    vkUnmapMemory(g_window.device, staging_buffer.device_memory);

    staging_buffer.mapped_memory = 0;
  }

  VkCommandBuffer command_buffer = vkutils_begin_command_buffer();

  VkBufferCopy buffer_copy = {
    .srcOffset = 0,
    .dstOffset = 0,
    .size = buffer_size,
  };

  vkCmdCopyBuffer(command_buffer, staging_buffer.handle, target_buffer.handle, 1, &buffer_copy);

  vkutils_end_command_buffer(command_buffer);

  buffer_destroy(&staging_buffer);

  return target_buffer;
}
buffer_t buffer_create_storage(void *buffer, uint64_t buffer_size) {
  buffer_t staging_buffer = buffer_create(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  buffer_t target_buffer = buffer_create(buffer_size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  if (buffer) {

    VK_CHECK(vkMapMemory(g_window.device, staging_buffer.device_memory, 0, staging_buffer.size, 0, &staging_buffer.mapped_memory));

    memcpy(staging_buffer.mapped_memory, buffer, buffer_size);

    vkUnmapMemory(g_window.device, staging_buffer.device_memory);

    staging_buffer.mapped_memory = 0;
  }

  VkCommandBuffer command_buffer = vkutils_begin_command_buffer();

  VkBufferCopy buffer_copy = {
    .srcOffset = 0,
    .dstOffset = 0,
    .size = buffer_size,
  };

  vkCmdCopyBuffer(command_buffer, staging_buffer.handle, target_buffer.handle, 1, &buffer_copy);

  vkutils_end_command_buffer(command_buffer);

  buffer_destroy(&staging_buffer);

  return target_buffer;
}

buffer_t buffer_create_vertex_coherent(void *buffer, uint64_t buffer_size) {
  buffer_t target_buffer = buffer_create(buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  VK_CHECK(vkMapMemory(g_window.device, target_buffer.device_memory, 0, target_buffer.size, 0, &target_buffer.mapped_memory));

  if (buffer) {
    memcpy(target_buffer.mapped_memory, buffer, buffer_size);
  }

  return target_buffer;
}
buffer_t buffer_create_index_coherent(void *buffer, uint64_t buffer_size) {
  buffer_t target_buffer = buffer_create(buffer_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  VK_CHECK(vkMapMemory(g_window.device, target_buffer.device_memory, 0, target_buffer.size, 0, &target_buffer.mapped_memory));

  if (buffer) {
    memcpy(target_buffer.mapped_memory, buffer, buffer_size);
  }

  return target_buffer;
}
buffer_t buffer_create_uniform_coherent(void *buffer, uint64_t buffer_size) {
  buffer_t target_buffer = buffer_create(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  VK_CHECK(vkMapMemory(g_window.device, target_buffer.device_memory, 0, target_buffer.size, 0, &target_buffer.mapped_memory));

  if (buffer) {
    memcpy(target_buffer.mapped_memory, buffer, buffer_size);
  }

  return target_buffer;
}
buffer_t buffer_create_storage_coherent(void *buffer, uint64_t buffer_size) {
  buffer_t target_buffer = buffer_create(buffer_size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  VK_CHECK(vkMapMemory(g_window.device, target_buffer.device_memory, 0, target_buffer.size, 0, &target_buffer.mapped_memory));

  if (buffer) {
    memcpy(target_buffer.mapped_memory, buffer, buffer_size);
  }

  return target_buffer;
}
