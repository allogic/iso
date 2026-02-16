#include <pch.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

uint8_t *imgutil_load_image_from_file(uint32_t *width, uint32_t *height, uint32_t *channel, char const *file_path) {
  int32_t w = 0;
  int32_t h = 0;
  int32_t c = 0;

  uint8_t *image = stbi_load(file_path, &w, &h, &c, 0);

  if (width) {
    *width = w;
  }

  if (height) {
    *height = h;
  }

  if (channel) {
    *channel = c;
  }

  return image;
}
uint8_t *imgutil_load_image_from_memory(uint32_t *width, uint32_t *height, uint32_t *channel, uint8_t *data, uint64_t size) {
  int32_t w = 0;
  int32_t h = 0;
  int32_t c = 0;

  uint8_t *image = stbi_load_from_memory(data, (uint32_t)size, &w, &h, &c, 0);

  if (width) {
    *width = w;
  }

  if (height) {
    *height = h;
  }

  if (channel) {
    *channel = c;
  }

  return image;
}

void imgutil_free_image(uint8_t *image) {
  stbi_image_free(image);
}
