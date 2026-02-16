#ifndef FSUTIL_H
#define FSUTIL_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void fsutil_load_text(uint8_t **buffer, uint64_t *buffer_size, char const *file_path);
void fsutil_load_binary(uint8_t **buffer, uint64_t *buffer_size, char const *file_path);

void fsutil_save_text(uint8_t *buffer, uint64_t buffer_size, char const *file_path);
void fsutil_save_binary(uint8_t *buffer, uint64_t buffer_size, char const *file_path);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // FSUTIL_H
