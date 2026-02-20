#include <pch.h>

tile_t const g_tile_lut[TILE_COUNT] = {
  {
    .tile_size_x = 1,
    .tile_size_z = 1,
    .width = 32,
    .height = 32,
    .flipbock_id = 0,
    .atlas_id = 0, // Empty
  },
  {
    .tile_size_x = 1,
    .tile_size_z = 1,
    .width = 32,
    .height = 32,
    .flipbock_id = 0,
    .atlas_id = 1, // Dirt
  },
  {
    .tile_size_x = 1,
    .tile_size_z = 1,
    .width = 32,
    .height = 32,
    .flipbock_id = 0,
    .atlas_id = 2, // Grass
  },
  {
    .tile_size_x = 1,
    .tile_size_z = 1,
    .width = 32,
    .height = 32,
    .flipbock_id = 0,
    .atlas_id = 3, // Conveyor Bottom
  },
  {
    .tile_size_x = 1,
    .tile_size_z = 1,
    .width = 32,
    .height = 32,
    .flipbock_id = 0,
    .atlas_id = 4, // Conveyor Top
  },
};
