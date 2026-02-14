#include <pch.h>

void fsutils_load_text(uint8_t **buffer, uint64_t *buffer_size, char const *file_path) {
  FILE *file = 0;

  fopen_s(&file, file_path, "r");

  if (file) {

    fseek(file, 0, SEEK_END);

    (*buffer_size) = (uint64_t)ftell(file);
    (*buffer) = (uint8_t *)HEAP_ALLOC((*buffer_size), 0, 0);

    fseek(file, 0, SEEK_SET);
    uint64_t read_count = fread(*buffer, sizeof(uint8_t), *buffer_size, file);

    (*buffer)[read_count] = 0;

    fclose(file);
  }
}
void fsutils_load_binary(uint8_t **buffer, uint64_t *buffer_size, char const *file_path) {
  FILE *file = 0;

  fopen_s(&file, file_path, "rb");

  if (file) {

    fseek(file, 0, SEEK_END);

    (*buffer_size) = (uint64_t)ftell(file);
    (*buffer) = (uint8_t *)HEAP_ALLOC(*buffer_size, 0, 0);

    fseek(file, 0, SEEK_SET);
    fread(*buffer, sizeof(uint8_t), *buffer_size, file);

    fclose(file);
  }
}

void fsutils_save_text(uint8_t *buffer, uint64_t buffer_size, char const *file_path) {
  FILE *file = 0;

  fopen_s(&file, file_path, "w");

  if (file) {

    fseek(file, 0, SEEK_SET);
    fwrite(buffer, sizeof(uint8_t), buffer_size, file);

    fclose(file);
  }
}
void fsutils_save_binary(uint8_t *buffer, uint64_t buffer_size, char const *file_path) {
  FILE *file = 0;

  fopen_s(&file, file_path, "wb");

  if (file) {

    fseek(file, 0, SEEK_SET);
    fwrite(buffer, sizeof(uint8_t), buffer_size, file);

    fclose(file);
  }
}
