#ifndef ISO_H
#define ISO_H

#define TILE_WIDTH (32.0F)
#define TILE_HEIGHT (32.0F)

#define ISO_X_SCALE (16.0F)
#define ISO_Y_SCALE (8.0F)
#define ISO_Y_HEIGHT (16.0F)

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void iso_screen_to_world(float screen_position_x, float screen_position_y, float camera_position_x, float camera_position_y, float *world_position_x, float *world_position_z);
void iso_world_to_screen(float world_position_x, float world_position_y, float world_position_z, float camera_position_x, float camera_position_y, float *screen_position_x, float *screen_position_y);

int8_t iso_point_in_diamond(float point_x, float point_y, float center_x, float center_y);
void iso_pick_tile(float mouse_position_x, float mouse_position_y, float camera_position_x, float camera_position_y, int32_t *iso_tile_x, int32_t *iso_tile_z);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // ISO_H
