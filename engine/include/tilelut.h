#ifndef TILELUT_H
#define TILELUT_H

#define TILE_COUNT (3)

typedef struct tile_t {
  float width;
  float height;
  float reserved0;
  uint32_t atlas_id;
} tile_t;

STATIC_ASSERT(ALIGNOF(tile_t) == 4);

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern tile_t const g_tile_lut[TILE_COUNT];

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TILELUT_H
