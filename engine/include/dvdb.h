#ifndef DVDB_H
#define DVDB_H

#define DVDB_DIM_X (4)
#define DVDB_DIM_Y (4)
#define DVDB_DIM_Z (4)

#define DVDB_CHUNK_SIZE (8)
#define DVDB_CHUNK_COUNT (64)

typedef struct dvdb_chunk_info_t {
  uint32_t is_dirty;
} dvdb_chunk_info_t;

STATIC_ASSERT(ALIGNOF(dvdb_chunk_info_t) == 4);

typedef struct dvdb_t {
  uint32_t geometry_count;
  uint32_t primitive_count;
  uint32_t instance_count;
  dvdb_chunk_info_t *chunk_info;
} dvdb_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern dvdb_t g_dvdb;

void dvdb_create(void);
void dvdb_update_descriptors(void);
void dvdb_draw(void);
void dvdb_debug(void);
void dvdb_destroy(void);

uint32_t dvdb_chunk_position_to_index(ivector3_t chunk_position);
ivector3_t dvdb_chunk_index_to_position(uint32_t chunk_index);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DVDB_H
