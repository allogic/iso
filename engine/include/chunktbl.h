#ifndef CHUNKTBL_H
#define CHUNKTBL_H

typedef struct chunktbl_t {
  uint32_t *index_to_handle;
  uint32_t *handle_to_index;
  uint32_t capacity;
  uint32_t dense_count;
} chunktbl_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void chunktbl_create(chunktbl_t *tbl, uint32_t capacity);
uint32_t chunktbl_lookup(chunktbl_t *tbl, uint32_t handle);
void chunktbl_insert(chunktbl_t *tbl, uint32_t handle);
void chunktbl_remove(chunktbl_t *tbl, uint32_t handle);
void chunktbl_dump(chunktbl_t *tbl);
void chunktbl_destroy(chunktbl_t *tbl);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHUNKTBL_H
