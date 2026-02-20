#ifndef PLAYER_H
#define PLAYER_H

typedef struct player_t {
  vector3_t position;
  vector3_t velocity;
  ivector3_t tile_position;
  float camera_zoom;
  float movement_speed_fast;
  float movement_speed_default;
  float linear_drag;
  uint32_t tile_id;
} player_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern player_t g_player;

void player_update(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PLAYER_H
