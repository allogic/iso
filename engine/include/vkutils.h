#ifndef VKUTILS_H
#define VKUTILS_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

uint32_t vkutils_find_memory_type_index(uint32_t type_filter, VkMemoryPropertyFlags memory_property_flags);

VkCommandBuffer vkutils_begin_command_buffer(void);
void vkutils_end_command_buffer(VkCommandBuffer command_buffer);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // VKUTILS_H
