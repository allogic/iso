#include <pch.h>

static uint32_t map_probe_distance(uint32_t slot, uint32_t ideal, uint32_t capacity) {
  return (slot + capacity - ideal) & (capacity - 1);
}

void map_create(map_t *map) {
  ASSERT((map->capacity & (map->capacity - 1)) == 0);

  map->entries = (map_entry_t *)HEAP_ALLOC(sizeof(map_entry_t) * map->capacity, 1, 0);
}
uint32_t *map_lookup(map_t *map, ivector3_t key) {
  uint32_t hash = ivector3_hash32(key);
  uint32_t mask = map->capacity - 1;
  uint32_t index = hash & mask;

  while (1) {

    map_entry_t *e = &map->entries[index];

    if (e->occupied == 0) {

      return 0;
    }

    if ((e->hash == hash) && (ivector3_equal(e->key, key))) {

      return &e->value;
    }

    index = (index + 1) & mask;
  }
}
void map_insert(map_t *map, ivector3_t key, uint32_t value) {
  ASSERT((map->count * 10) < (map->capacity * 7));

  uint32_t hash = ivector3_hash32(key);
  uint32_t mask = map->capacity - 1;
  uint32_t index = hash & mask;

  map_entry_t entry = {
    .key = key,
    .value = value,
    .hash = hash,
    .occupied = 1,
  };

  while (1) {

    map_entry_t *e = &map->entries[index];

    if (e->occupied == 0) {

      *e = entry;
      map->count++;

      return;
    }

    if ((e->hash == hash) && ivector3_equal(e->key, key)) {

      e->value = value;

      return;
    }

    uint32_t curr_distance = map_probe_distance(index, e->hash & mask, map->capacity);
    uint32_t new_distance = map_probe_distance(index, hash & mask, map->capacity);

    if (new_distance > curr_distance) {

      map_entry_t tmp = *e;
      *e = entry;
      entry = tmp;
    }

    index = (index + 1) & mask;
  }
}
void map_remove(map_t *map, ivector3_t key) {
  uint32_t hash = ivector3_hash32(key);
  uint32_t mask = map->capacity - 1;
  uint32_t index = hash & mask;

  while (1) {

    map_entry_t *e = &map->entries[index];

    if (e->occupied == 0) {

      return;
    }

    if ((e->hash == hash) && ivector3_equal(e->key, key)) {

      break;
    }

    index = (index + 1) & mask;
  }

  uint32_t curr = index;
  uint32_t next = (curr + 1) & mask;

  while (1) {

    map_entry_t *e = &map->entries[next];

    if (e->occupied == 0) {

      break;
    }

    if (map_probe_distance(next, e->hash & mask, map->capacity) == 0) {

      break;
    }

    map->entries[curr] = *e;
    curr = next;

    next = (next + 1) & mask;
  }

  map->entries[curr].occupied = 0;
  map->count--;
}
void map_iterate(map_t *map, map_proc_t proc) {
  int32_t index = 0;
  int32_t count = map->capacity;

  while (index < count) {

    map_entry_t *e = &map->entries[index];

    if (e->occupied) {

      proc(e->key, e->value);
    }

    index++;
  }
}
void map_clear(map_t *map) {
  map->count = 0;

  memset(map->entries, 0, sizeof(map_entry_t) * map->capacity);
}
void map_destroy(map_t *map) {
  HEAP_FREE(map->entries);
}
