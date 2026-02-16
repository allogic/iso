#include <pch.h>

uint32_t vkutil_find_memory_type_index(uint32_t type_filter, VkMemoryPropertyFlags memory_property_flags) {
  uint32_t memory_type_index = 0;
  uint32_t memory_type_count = g_window.physical_device_memory_properties.memoryTypeCount;

  while (memory_type_index < memory_type_count) {

    if ((type_filter & (1 << memory_type_index)) && ((g_window.physical_device_memory_properties.memoryTypes[memory_type_index].propertyFlags & memory_property_flags) == memory_property_flags)) {
      return memory_type_index;
    }

    memory_type_index++;
  }

  return UINT32_MAX;
}
