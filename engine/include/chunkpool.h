#ifndef CHUNKPOOL_H
#define CHUNKPOOL_H

typedef struct chunkpool_t {
  chunk_t *chunk;
  uint32_t *free_stack;
  uint32_t capacity;
  uint32_t free_count;
} chunkpool_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void chunkpool_create(chunkpool_t *pool);
uint32_t chunkpool_alloc(chunkpool_t *pool);
chunk_t *chunkpool_lookup(chunkpool_t *pool, uint32_t handle);
void chunkpool_free(chunkpool_t *pool, uint32_t handle);
void chunkpool_destroy(chunkpool_t *pool);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHUNKPOOL_H
