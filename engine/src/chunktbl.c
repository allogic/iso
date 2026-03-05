#include <pch.h>

void chunktbl_create(chunktbl_t *tbl, uint32_t capacity) {
  tbl->capacity = capacity;
  tbl->index_to_handle = (uint32_t *)HEAP_ALLOC(sizeof(uint32_t) * capacity, 1, 0);
  tbl->handle_to_index = (uint32_t *)HEAP_ALLOC(sizeof(uint32_t) * capacity, 1, 0);
}
uint32_t chunktbl_lookup(chunktbl_t *tbl, uint32_t handle) {
  return tbl->handle_to_index[handle];
}
void chunktbl_insert(chunktbl_t *tbl, uint32_t handle) {
  uint32_t index = tbl->dense_count;

  tbl->index_to_handle[index] = handle;
  tbl->handle_to_index[handle] = index;
  tbl->dense_count++;
}
void chunktbl_remove(chunktbl_t *tbl, uint32_t handle) {
  uint32_t index = tbl->handle_to_index[handle];
  uint32_t last_index = tbl->dense_count - 1;
  uint32_t last_handle = tbl->index_to_handle[last_index];

  tbl->index_to_handle[index] = last_handle;
  tbl->handle_to_index[last_handle] = index;
  tbl->dense_count--;
}
void chunktbl_dump(chunktbl_t *tbl) {
  printf("Chunktbl Entries\n");

  uint32_t index = 0;
  uint32_t count = tbl->dense_count;

  while (index < count) {

    uint32_t handle = tbl->index_to_handle[index];

    printf("  Index: %6u | Handle: %6u\n", index, handle);

    index++;
  }

  printf("\n");
}
void chunktbl_destroy(chunktbl_t *tbl) {
  HEAP_FREE(tbl->index_to_handle);
  HEAP_FREE(tbl->handle_to_index);
}
