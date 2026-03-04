#ifndef CONTAINER_MAP_H
#define CONTAINER_MAP_H

typedef void (*map_proc_t)(ivector3_t key, uint32_t value);

typedef struct map_entry_t {
  ivector3_t key;
  uint32_t value;
  uint32_t hash;
  uint32_t occupied;
} map_entry_t;

typedef struct map_t {
  map_entry_t *entries;
  uint32_t capacity;
  uint32_t count;
} map_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void map_create(map_t *map);
uint32_t *map_lookup(map_t *map, ivector3_t key);
void map_insert(map_t *map, ivector3_t key, uint32_t value);
void map_remove(map_t *map, ivector3_t key);
void map_iterate(map_t *map, map_proc_t proc);
void map_clear(map_t *map);
void map_destroy(map_t *map);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CONTAINER_MAP_H
