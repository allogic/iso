#include <pch.h>

player_t g_player = {0};

void player_create(void) {
  g_player.position = vector2_zero();
  g_player.camera_zoom = 1.0;
  g_player.movement_speed_fast = 100000.0F;
  g_player.movement_speed_default = 10000.0F;
  g_player.linear_drag = 25.0F;
  g_player.linear_velocity = vector2_zero();
}
void player_update(void) {
  float speed = window_is_keyboard_key_held(KEYBOARD_KEY_LEFT_SHIFT)
                  ? g_player.movement_speed_fast
                  : g_player.movement_speed_default;

  if (window_is_keyboard_key_held(KEYBOARD_KEY_D)) {
    g_player.linear_velocity.x -= speed * g_window.delta_time;
  }

  if (window_is_keyboard_key_held(KEYBOARD_KEY_A)) {
    g_player.linear_velocity.x += speed * g_window.delta_time;
  }

  if (window_is_keyboard_key_held(KEYBOARD_KEY_W)) {
    g_player.linear_velocity.y -= speed * g_window.delta_time;
  }

  if (window_is_keyboard_key_held(KEYBOARD_KEY_S)) {
    g_player.linear_velocity.y += speed * g_window.delta_time;
  }

  float linear_damping = expf(-g_player.linear_drag * g_window.delta_time);

  g_player.position = vector2_add(g_player.position, vector2_muls(g_player.linear_velocity, g_window.delta_time));
  g_player.linear_velocity = vector2_muls(g_player.linear_velocity, linear_damping);
}
