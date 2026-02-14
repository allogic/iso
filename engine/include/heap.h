#ifndef HEAP_H
#define HEAP_H

#define ENABLE_HEAP_TRACE

typedef struct mem_block_t {
  char const *file_name;
  char const *function_name;
  uint64_t line_number;
  uint64_t block_size;
  time_t time_stamp;
} mem_block_t;

#ifdef BUILD_DEBUG
#  define HEAP_ALLOC(BLOCK_SIZE, ZERO_BLOCK, BLOCK_REF) heap_alloc(__FILE__, __func__, __LINE__, BLOCK_SIZE, ZERO_BLOCK, BLOCK_REF)
#else
#  define HEAP_ALLOC(BLOCK_SIZE, ZERO_BLOCK, BLOCK_REF) heap_alloc(BLOCK_SIZE, ZERO_BLOCK, BLOCK_REF)
#endif // BUILD_DEBUG

#define HEAP_FREE(BLOCK) heap_free(BLOCK)

#ifdef BUILD_DEBUG
#  define HEAP_RESET() heap_reset()
#else
#  define HEAP_RESET()
#endif // BUILD_DEBUG

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifdef BUILD_DEBUG
void *heap_alloc(char const *file_name, char const *function_name, uint64_t line_number, uint64_t block_size, uint8_t zero_block, void const *block_ref);
#else
void *heap_alloc(uint64_t block_size, uint8_t zero_block, void const *block_ref);
#endif // BUILD_DEBUG

void heap_free(void *block);

#ifdef BUILD_DEBUG
void heap_reset(void);
#endif // BUILD_DEBUG

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // HEAP_H
