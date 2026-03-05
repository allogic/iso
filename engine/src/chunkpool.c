#include <pch.h>

void chunkpool_create(chunkpool_t *pool, uint32_t capacity) {
  pool->capacity = capacity;
  pool->free_count = capacity;
  pool->free_stack = (uint32_t *)HEAP_ALLOC(sizeof(uint32_t) * capacity, 1, 0);

  uint32_t index = 0;
  uint32_t count = capacity;

  while (index < count) {

    pool->free_stack[index] = (capacity - 1) - index;

    index++;
  }
}
uint32_t chunkpool_alloc(chunkpool_t *pool) {
  ASSERT(pool->free_count > 0);

  return pool->free_stack[--pool->free_count];
}
void chunkpool_free(chunkpool_t *pool, uint32_t handle) {
  ASSERT(handle < pool->capacity);

  pool->free_stack[pool->free_count++] = handle;
}
void chunkpool_destroy(chunkpool_t *pool) {
  HEAP_FREE(pool->free_stack);
}
