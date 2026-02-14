#ifndef FSUTILS_H
#define FSUTILS_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void fsutils_load_text(uint8_t **buffer, uint64_t *buffer_size, char const *file_path);
void fsutils_load_binary(uint8_t **buffer, uint64_t *buffer_size, char const *file_path);

void fsutils_save_text(uint8_t *buffer, uint64_t buffer_size, char const *file_path);
void fsutils_save_binary(uint8_t *buffer, uint64_t buffer_size, char const *file_path);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // FSUTILS_H
