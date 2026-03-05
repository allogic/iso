#include <pch.h>

static uint32_t chunkmap_probe_distance(uint32_t slot, uint32_t ideal, uint32_t capacity) {
  return (slot + capacity - ideal) & (capacity - 1);
}

void chunkmap_create(chunkmap_t *map) {
  ASSERT((map->capacity & (map->capacity - 1)) == 0);

  map->entry = (chunkmap_entry_t *)HEAP_ALLOC(sizeof(chunkmap_entry_t) * map->capacity, 1, 0);
}
uint32_t *chunkmap_lookup(chunkmap_t *map, ivector3_t position) {
  uint32_t hash = ivector3_hash32(position);
  uint32_t mask = map->capacity - 1;
  uint32_t index = hash & mask;

  while (1) {

    chunkmap_entry_t *e = &map->entry[index];

    if (e->occupied == 0) {

      return 0;
    }

    if ((e->hash == hash) && (ivector3_equal(e->position, position))) {

      return &e->handle;
    }

    index = (index + 1) & mask;
  }
}
void chunkmap_insert(chunkmap_t *map, ivector3_t position, uint32_t handle) {
  ASSERT((map->count * 10) < (map->capacity * 7));

  uint32_t hash = ivector3_hash32(position);
  uint32_t mask = map->capacity - 1;
  uint32_t index = hash & mask;

  chunkmap_entry_t entry = {
    .position = position,
    .handle = handle,
    .hash = hash,
    .occupied = 1,
  };

  while (1) {

    chunkmap_entry_t *e = &map->entry[index];

    if (e->occupied == 0) {

      *e = entry;
      map->count++;

      return;
    }

    if ((e->hash == hash) && ivector3_equal(e->position, position)) {

      e->handle = handle;

      return;
    }

    uint32_t curr_distance = chunkmap_probe_distance(index, e->hash & mask, map->capacity);
    uint32_t new_distance = chunkmap_probe_distance(index, hash & mask, map->capacity);

    if (new_distance > curr_distance) {

      chunkmap_entry_t tmp = *e;
      *e = entry;
      entry = tmp;
    }

    index = (index + 1) & mask;
  }
}
void chunkmap_remove(chunkmap_t *map, ivector3_t position) {
  uint32_t hash = ivector3_hash32(position);
  uint32_t mask = map->capacity - 1;
  uint32_t index = hash & mask;

  while (1) {

    chunkmap_entry_t *e = &map->entry[index];

    if (e->occupied == 0) {

      return;
    }

    if ((e->hash == hash) && ivector3_equal(e->position, position)) {

      break;
    }

    index = (index + 1) & mask;
  }

  uint32_t curr = index;
  uint32_t next = (curr + 1) & mask;

  while (1) {

    chunkmap_entry_t *e = &map->entry[next];

    if (e->occupied == 0) {

      break;
    }

    if (chunkmap_probe_distance(next, e->hash & mask, map->capacity) == 0) {

      break;
    }

    map->entry[curr] = *e;
    curr = next;

    next = (next + 1) & mask;
  }

  map->entry[curr].occupied = 0;
  map->count--;
}
void chunkmap_iterate(chunkmap_t *map, chunkmap_proc_t proc) {
  uint32_t index = 0;
  uint32_t count = map->capacity;

  while (index < count) {

    chunkmap_entry_t *e = &map->entry[index];

    if (e->occupied) {

      proc(e->position, e->handle);
    }

    index++;
  }
}
void chunkmap_clear(chunkmap_t *map) {
  map->count = 0;

  memset(map->entry, 0, sizeof(chunkmap_entry_t) * map->capacity);
}
void chunkmap_dump(chunkmap_t *map) {
  printf("Chunkmap Entries\n");

  uint32_t index = 0;
  uint32_t count = map->capacity;

  while (index < count) {

    chunkmap_entry_t *e = &map->entry[index];

    printf("  Index: %6u | Handle: %6u | Occupied: %6u\n", index, e->handle, e->occupied);

    index++;
  }

  printf("\n");
}
void chunkmap_destroy(chunkmap_t *map) {
  HEAP_FREE(map->entry);
}
