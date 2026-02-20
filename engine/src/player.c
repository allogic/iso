#include <pch.h>

static void player_update_position(void);
static void player_update_selection(void);

player_t g_player = {
  .position = {
    .x = 0.0F,
    .y = 15.0F,
    .z = 0.0F,
  },
  .tile_position = {
    .x = 0,
    .y = 15,
    .z = 0,
  },
  .camera_zoom = 1.0,
  .movement_speed_fast = 1000.0F,
  .movement_speed_default = 100.0F,
  .linear_drag = 25.0F,
};

void player_update(void) {
  player_update_position();
  player_update_selection();
}

static void player_update_position(void) {
  float speed = window_is_keyboard_key_held(KEYBOARD_KEY_LEFT_SHIFT)
                  ? g_player.movement_speed_fast
                  : g_player.movement_speed_default;

  if (window_is_keyboard_key_held(KEYBOARD_KEY_D)) {
    g_player.velocity.x -= speed * g_window.delta_time;
  }

  if (window_is_keyboard_key_held(KEYBOARD_KEY_A)) {
    g_player.velocity.x += speed * g_window.delta_time;
  }

  if (window_is_keyboard_key_held(KEYBOARD_KEY_W)) {
    g_player.velocity.z -= speed * g_window.delta_time;
  }

  if (window_is_keyboard_key_held(KEYBOARD_KEY_S)) {
    g_player.velocity.z += speed * g_window.delta_time;
  }

  float linear_damping = expf(-g_player.linear_drag * g_window.delta_time);

  g_player.position = vector3_add(g_player.position, vector3_muls(g_player.velocity, g_window.delta_time));
  g_player.velocity = vector3_muls(g_player.velocity, linear_damping);
}
static void player_update_selection(void) {
  float mouse_position_x = (float)g_window.mouse_position_x;
  float mouse_position_y = (float)g_window.mouse_position_y;

  float camera_position_x = g_player.position.x; // THIS
  float camera_position_y = g_player.position.y; // THIS

  iso_pick_tile(mouse_position_x, mouse_position_y, camera_position_x, camera_position_y, &g_player.tile_position.x, &g_player.tile_position.z);

  g_player.tile_position.y -= g_window.mouse_wheel_delta;

  // TODO: make it this pretty..
  {
    float screen_space_x = 0.0F;
    float screen_space_y = 0.0F;

    iso_world_to_screen((float)g_player.tile_position.x + 0.5F, (float)g_player.tile_position.y, (float)g_player.tile_position.z + 0.5F, camera_position_x, camera_position_y, &screen_space_x, &screen_space_y);

    vector2_t pivot = {(float)g_window.window_width * 0.5F, (float)g_window.window_height * 0.5F};

    screen_space_x = pivot.x + (screen_space_x - pivot.x) * g_player.camera_zoom;
    screen_space_y = pivot.y + (screen_space_y - pivot.y) * g_player.camera_zoom;

    vector2_t top = {screen_space_x, screen_space_y - ISO_Y_SCALE};
    vector2_t right = {screen_space_x + ISO_X_SCALE, screen_space_y};
    vector2_t bottom = {screen_space_x, screen_space_y + ISO_Y_SCALE};
    vector2_t left = {screen_space_x - ISO_X_SCALE, screen_space_y};

    vector4_t color = {1.0F, 1.0F, 1.0F, 1.0F};

    renderer_draw_debug_line(top, right, color);
    renderer_draw_debug_line(right, bottom, color);
    renderer_draw_debug_line(bottom, left, color);
    renderer_draw_debug_line(left, top, color);
  }
}
