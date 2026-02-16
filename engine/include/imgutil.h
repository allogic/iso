#ifndef IMGUTIL_H
#define IMGUTIL_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

uint8_t *imgutil_load_image_from_file(uint32_t *width, uint32_t *height, uint32_t *channel, char const *file_path);
uint8_t *imgutil_load_image_from_memory(uint32_t *width, uint32_t *height, uint32_t *channel, uint8_t *data, uint64_t size);

void imgutil_free_image(uint8_t *image);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // IMGUTIL_H
