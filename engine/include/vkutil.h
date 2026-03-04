#ifndef VKUTIL_H
#define VKUTIL_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

uint32_t vkutil_find_memory_type_index(uint32_t type_filter, VkMemoryPropertyFlags memory_property_flags);

VkCommandBuffer vkutil_primary_command_buffer_record_immediate(void);
void vkutil_primary_command_buffer_submit_immediate(VkCommandBuffer command_buffer);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // VKUTIL_H
