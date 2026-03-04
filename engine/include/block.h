#ifndef BLOCK_H
#define BLOCK_H

#define BLOCK_COUNT (2)

typedef enum block_type_t {
  BLOCK_TYPE_DIRT = 0,
  BLOCK_TYPE_GRASS,
  BLOCK_TYPE_GRASSY_DIRT,
  BLOCK_TYPE_STONE,
  BLOCK_TYPE_COUNT,
} block_type_t;

typedef enum block_face_t {
  BLOCK_FACE_NORTH = 0,
  BLOCK_FACE_SOUTH,
  BLOCK_FACE_WEST,
  BLOCK_FACE_EAST,
  BLOCK_FACE_TOP,
  BLOCK_FACE_BOTTOM,
  BLOCK_FACE_COUNT,
} block_face_t;

typedef struct block_t {
  block_type_t block_type;
  uint32_t block_face_atlas_id[BLOCK_FACE_COUNT];
} block_t;

STATIC_ASSERT(ALIGN_OF(block_t) == 4);

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern block_t g_block[BLOCK_TYPE_COUNT];

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // BLOCK_H
