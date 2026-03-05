#ifndef CHUNKMAP_H
#define CHUNKMAP_H

typedef void (*chunkmap_proc_t)(ivector3_t position, uint32_t handle);

typedef struct chunkmap_entry_t {
  ivector3_t position;
  uint32_t handle;
  uint32_t hash;
  uint32_t occupied;
} chunkmap_entry_t;

typedef struct chunkmap_t {
  chunkmap_entry_t *entry;
  uint32_t capacity;
  uint32_t count;
} chunkmap_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void chunkmap_create(chunkmap_t *map);
uint32_t *chunkmap_lookup(chunkmap_t *map, ivector3_t position);
void chunkmap_insert(chunkmap_t *map, ivector3_t position, uint32_t handle);
void chunkmap_remove(chunkmap_t *map, ivector3_t position);
void chunkmap_iterate(chunkmap_t *map, chunkmap_proc_t proc);
void chunkmap_clear(chunkmap_t *map);
void chunkmap_dump(chunkmap_t *map);
void chunkmap_destroy(chunkmap_t *map);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHUNKMAP_H
