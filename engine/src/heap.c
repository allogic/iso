#include <pch.h>

#ifdef BUILD_DEBUG
uint64_t g_heap_allocated_blocks = 0;
uint64_t g_heap_allocated_bytes = 0;
#endif // BUILD_DEBUG

#ifdef BUILD_DEBUG
void *heap_alloc(char const *file_name, char const *function_name, uint64_t line_number, uint64_t block_size, uint8_t zero_block, void const *block_ref) {
  g_heap_allocated_blocks += 1;
  g_heap_allocated_bytes += sizeof(mem_block_t) + block_size;

  mem_block_t *mem_block = (mem_block_t *)malloc(sizeof(mem_block_t) + block_size);

  mem_block->file_name = file_name;
  mem_block->function_name = function_name;
  mem_block->line_number = line_number;
  mem_block->block_size = block_size;
  mem_block->time_stamp = time(0);

#  ifdef ENABLE_HEAP_TRACE
  char time_stamp_buffer[64];

  strftime(time_stamp_buffer, sizeof(time_stamp_buffer), "%Y-%m-%d %H:%M:%S", localtime(&mem_block->time_stamp));

  printf("[%s] %s:%zu %zu bytes allocated\n", time_stamp_buffer, mem_block->function_name, mem_block->line_number, mem_block->block_size);
#  endif // ENABLE_HEAP_TRACE

  mem_block += 1;

  if (zero_block) {
    memset(mem_block, 0, block_size);
  }

  if (block_ref) {
    memcpy(mem_block, block_ref, block_size);
  }

  return mem_block;
}
#else
void *heap_alloc(uint64_t block_size, uint8_t zero_block, void const *block_ref) {
  void *block = malloc(block_size);

  if (zero_block) {
    memset(block, 0, block_size);
  }

  if (block_ref) {
    memcpy(block, block_ref, block_size);
  }

  return block;
}
#endif // BUILD_DEBUG

void heap_free(void *block) {
#ifdef BUILD_DEBUG
  mem_block_t *mem_block = (mem_block_t *)block;

  mem_block -= 1;

  g_heap_allocated_blocks -= 1;
  g_heap_allocated_bytes -= sizeof(mem_block_t) + mem_block->block_size;

  free(mem_block);
#else
  free(block);
#endif // BUILD_DEBUG
}

#ifdef BUILD_DEBUG
void heap_reset(void) {
  if (g_heap_allocated_blocks || g_heap_allocated_bytes) {
    printf("%zu blocks with %zu bytes leaked\n", g_heap_allocated_blocks, g_heap_allocated_bytes);
  }

  g_heap_allocated_blocks = 0;
  g_heap_allocated_bytes = 0;
}
#endif // BUILD_DEBUG
