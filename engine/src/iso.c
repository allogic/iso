#include <pch.h>

static int32_t const s_tile_offset[4][2] = {
  {1, 0},
  {-1, 0},
  {0, 1},
  {0, -1},
};

void iso_screen_to_world(float screen_position_x, float screen_position_y, float camera_position_x, float camera_position_y, float *world_position_x, float *world_position_z) {
  screen_position_x -= camera_position_x;
  screen_position_y -= camera_position_y;

  *world_position_x = ((screen_position_x / ISO_X_SCALE) + (screen_position_y / ISO_Y_SCALE)) * 0.5F;
  *world_position_z = ((screen_position_y / ISO_Y_SCALE) - (screen_position_x / ISO_X_SCALE)) * 0.5F;
}
void iso_world_to_screen(float world_position_x, float world_position_y, float world_position_z, float camera_position_x, float camera_position_y, float *screen_position_x, float *screen_position_y) {
  *screen_position_x = world_position_x * ISO_X_SCALE + world_position_z * -ISO_X_SCALE + camera_position_x;
  *screen_position_y = world_position_x * ISO_Y_SCALE + world_position_z * ISO_Y_SCALE - world_position_y * ISO_Y_HEIGHT + camera_position_y;
};

int8_t iso_point_in_diamond(float point_x, float point_y, float center_x, float center_y) {
  float dx = fabsf(point_x - center_x);
  float dy = fabsf(point_y - center_y);

  float nx = dx / (TILE_WIDTH * 0.5F);
  float ny = dy / (TILE_HEIGHT * 0.5F);

  return (nx + ny) <= 1.0F;
}

void iso_pick_tile(float mouse_position_x, float mouse_position_y, float camera_position_x, float camera_position_y, int32_t *tile_x, int32_t *tile_z) {
  float world_position_x = 0.0F;
  float world_position_z = 0.0F;

  iso_screen_to_world(mouse_position_x, mouse_position_y, camera_position_x, camera_position_y, &world_position_x, &world_position_z);

  *tile_x = (int32_t)floorf(world_position_x);
  *tile_z = (int32_t)floorf(world_position_z);

  float tile_center_x = 0.0F;
  float tile_center_y = 0.0F;

  iso_world_to_screen((float)(*tile_x) + 0.5F, 0.0F, (float)(*tile_z) + 0.5F, camera_position_x, camera_position_y, &tile_center_x, &tile_center_y);

  if (iso_point_in_diamond(mouse_position_x, mouse_position_y, tile_center_x, tile_center_y) == 0) {
    int32_t offset_index = 0;
    int32_t offset_count = 4;

    while (offset_index < offset_count) {

      int32_t neighbor_tile_x = *tile_x + s_tile_offset[offset_index][0];
      int32_t neighbor_tile_z = *tile_z + s_tile_offset[offset_index][1];

      float neighbor_tile_center_x = 0.0F;
      float neighbor_tile_center_y = 0.0F;

      iso_world_to_screen(neighbor_tile_x + 0.5F, 0.0F, neighbor_tile_z + 0.5F, camera_position_x, camera_position_y, &neighbor_tile_center_x, &neighbor_tile_center_y);

      if (iso_point_in_diamond(mouse_position_x, mouse_position_y, neighbor_tile_center_x, neighbor_tile_center_y)) {

        *tile_x = neighbor_tile_x;
        *tile_z = neighbor_tile_z;

        break;
      }

      offset_index++;
    }
  }
}
