#include <pch.h>

block_t g_block[BLOCK_TYPE_COUNT] = {
  {
    .block_type = BLOCK_TYPE_DIRT,
    .block_face_atlas_id = {
      0,
      0,
      0,
      0,
      0,
      0,
    },
  },
  {
    .block_type = BLOCK_TYPE_GRASS,
    .block_face_atlas_id = {
      2,
      2,
      2,
      2,
      2,
      2,
    },
  },
  {
    .block_type = BLOCK_TYPE_GRASSY_DIRT,
    .block_face_atlas_id = {
      1,
      2, // Top
      1,
      1,
      0, // Bottom
      1,
    },
  },
  {
    .block_type = BLOCK_TYPE_STONE,
    .block_face_atlas_id = {
      3,
      3,
      3,
      3,
      3,
      3,
    },
  },
};
