#ifndef VKUTIL_H
#define VKUTIL_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

uint32_t vkutil_find_memory_type_index(uint32_t type_filter, VkMemoryPropertyFlags memory_property_flags);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // VKUTIL_H
