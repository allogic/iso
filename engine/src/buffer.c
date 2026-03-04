#include <pch.h>

void buffer_create(buffer_t *buffer) {
  buffer->buffer_usage_flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  buffer_t staging_buffer = {
    .host_data = buffer->host_data,
    .size = buffer->size,
    .buffer_usage_flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    .memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
  };

  {
    VkBufferCreateInfo buffer_create_info = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size = buffer->size,
      .usage = buffer->buffer_usage_flags,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    VK_CHECK(vkCreateBuffer(g_window.device, &buffer_create_info, 0, &buffer->buffer_handle));

    VkMemoryRequirements memory_requirements = {0};

    vkGetBufferMemoryRequirements(g_window.device, buffer->buffer_handle, &memory_requirements);

    uint32_t memory_type_index = vkutil_find_memory_type_index(memory_requirements.memoryTypeBits, buffer->memory_property_flags);

    VkMemoryAllocateFlagsInfo memory_allocate_flags_info = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
      .flags = buffer->memory_allocate_flags,
    };

    VkMemoryAllocateInfo memory_allocate_info = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .pNext = &memory_allocate_flags_info,
      .allocationSize = memory_requirements.size,
      .memoryTypeIndex = memory_type_index,
    };

    VK_CHECK(vkAllocateMemory(g_window.device, &memory_allocate_info, 0, &buffer->device_memory));
    VK_CHECK(vkBindBufferMemory(g_window.device, buffer->buffer_handle, buffer->device_memory, 0));
  }

  {
    VkBufferCreateInfo buffer_create_info = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size = staging_buffer.size,
      .usage = staging_buffer.buffer_usage_flags,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    VK_CHECK(vkCreateBuffer(g_window.device, &buffer_create_info, 0, &staging_buffer.buffer_handle));

    VkMemoryRequirements memory_requirements = {0};

    vkGetBufferMemoryRequirements(g_window.device, staging_buffer.buffer_handle, &memory_requirements);

    uint32_t memory_type_index = vkutil_find_memory_type_index(memory_requirements.memoryTypeBits, staging_buffer.memory_property_flags);

    VkMemoryAllocateInfo memory_allocate_info = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize = memory_requirements.size,
      .memoryTypeIndex = memory_type_index,
    };

    VK_CHECK(vkAllocateMemory(g_window.device, &memory_allocate_info, 0, &staging_buffer.device_memory));
    VK_CHECK(vkBindBufferMemory(g_window.device, staging_buffer.buffer_handle, staging_buffer.device_memory, 0));
  }

  if (staging_buffer.host_data) {

    VK_CHECK(vkMapMemory(g_window.device, staging_buffer.device_memory, 0, staging_buffer.size, 0, &staging_buffer.device_data));

    memcpy(staging_buffer.device_data, staging_buffer.host_data, staging_buffer.size);

    vkUnmapMemory(g_window.device, staging_buffer.device_memory);

    staging_buffer.device_data = 0;
  }

  VkCommandBuffer command_buffer = vkutil_primary_command_buffer_record_immediate();

  if (buffer->zero_data) {

    vkCmdFillBuffer(command_buffer, buffer->buffer_handle, 0, buffer->size, 0);
  }

  VkBufferCopy buffer_copy = {
    .srcOffset = 0,
    .dstOffset = 0,
    .size = buffer->size,
  };

  vkCmdCopyBuffer(command_buffer, staging_buffer.buffer_handle, buffer->buffer_handle, 1, &buffer_copy);

  vkutil_primary_command_buffer_submit_immediate(command_buffer);

  vkFreeMemory(g_window.device, staging_buffer.device_memory, 0);
  vkDestroyBuffer(g_window.device, staging_buffer.buffer_handle, 0);
}
void buffer_map(buffer_t *buffer) {
  VK_CHECK(vkMapMemory(g_window.device, buffer->device_memory, 0, buffer->size, 0, &buffer->device_data));
}
void buffer_unmap(buffer_t *buffer) {
  vkUnmapMemory(g_window.device, buffer->device_memory);

  buffer->device_data = 0;
}
void buffer_destroy(buffer_t *buffer) {
  if (buffer->device_data) {

    vkUnmapMemory(g_window.device, buffer->device_memory);

    buffer->device_data = 0;
  }

  vkFreeMemory(g_window.device, buffer->device_memory, 0);
  vkDestroyBuffer(g_window.device, buffer->buffer_handle, 0);
}
