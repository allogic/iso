#ifndef PLAYER_H
#define PLAYER_H

typedef struct player_t {
  vector2_t position;
  float camera_zoom;
  float movement_speed_fast;
  float movement_speed_default;
  float linear_drag;
  vector2_t linear_velocity;
} player_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern player_t g_player;

void player_create(void);
void player_update(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PLAYER_H
