#include <pch.h>

void chunkpool_create(chunkpool_t *pool) {
  pool->chunk = (chunk_t *)HEAP_ALLOC(sizeof(chunk_t) * pool->capacity, 1, 0);
  pool->free_stack = (uint32_t *)HEAP_ALLOC(sizeof(uint32_t) * pool->capacity, 1, 0);

  uint32_t index = 0;
  uint32_t count = pool->capacity;

  while (index < count) {

    pool->free_stack[index] = (pool->capacity - 1) - index;

    index++;
  }
}
uint32_t chunkpool_alloc(chunkpool_t *pool) {
  ASSERT(pool->free_count > 0);

  return pool->free_stack[--pool->free_count];
}
chunk_t *chunkpool_lookup(chunkpool_t *pool, uint32_t handle) {
  ASSERT(handle < pool->capacity);

  return &pool->chunk[handle];
}
void chunkpool_free(chunkpool_t *pool, uint32_t handle) {
  ASSERT(handle < pool->capacity);

  pool->free_stack[pool->free_count++] = handle;
}
void chunkpool_destroy(chunkpool_t *pool) {
  HEAP_FREE(pool->chunk);
  HEAP_FREE(pool->free_stack);
}
