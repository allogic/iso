#ifndef PLAYER_H
#define PLAYER_H

typedef struct player_t {
  transform_t transform;
  camera_t camera;
  vector3_t mouse_begin;
  vector3_t mouse_end;
  vector3_t mouse_delta;
  vector3_t linear_velocity;
  vector3_t angular_velocity;
  float keyboard_speed_fast;
  float keyboard_speed_default;
  float mouse_rotation_speed;
  float linear_drag;
  float angular_drag;
} player_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern player_t g_player_0;

player_t player_create(void);
void player_update(player_t *player);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PLAYER_H
