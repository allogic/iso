#include <pch.h>

static void player_handle_position(void);
static void player_handle_rotation(void);

static void player_handle_linear_velocity(void);
static void player_handle_angular_velocity(void);

player_t g_player = {
  .transform = {
    .local_position = {0.0F, 0.0F, 0.0F},
    .local_rotation = {0.0F, 0.0F, 0.0F, 1.0},
    .local_scale = {1.0F, 1.0F, 1.0F},
    .world_position = {0.0F, 0.0F, 0.0F},
    .world_rotation = {0.0F, 0.0F, 0.0F, 1.0},
    .world_scale = {1.0F, 1.0F, 1.0F},
  },
  .camera = {
    .is_debug_enabled = 1,
    .is_orthographic = 0,
    .fov = 45.0F * DEG_TO_RAD,
    .zoom = 30.0F,
    .persp_near_z = 0.01F,
    .persp_far_z = 1000.0F,
    .ortho_near_z = -1000.0F,
    .ortho_far_z = 1000.0F,
  },
  .keyboard_speed_fast = 1000.0F,
  .keyboard_speed_default = 100.0F,
  .mouse_rotation_speed = 3.0F * DEG_TO_RAD,
  .linear_drag = 25.0F,
  .angular_drag = 35.0F,
};

void player_update(void) {
  player_handle_position();
  player_handle_rotation();

  player_handle_linear_velocity();
  player_handle_angular_velocity();

  transform_compute_world_position(&g_player.transform);
  transform_compute_world_rotation(&g_player.transform);
  transform_compute_world_scale(&g_player.transform);

  camera_update(&g_player.camera, &g_player.transform);
}

static void player_handle_position() {
  float speed = window_is_keyboard_key_held(KEYBOARD_KEY_LEFT_SHIFT)
                  ? g_player.keyboard_speed_fast
                  : g_player.keyboard_speed_default;

  vector3_t velocity = g_player.linear_velocity;

  if (window_is_keyboard_key_held(KEYBOARD_KEY_D)) {
    velocity = vector3_add(velocity, vector3_muls(transform_local_right(&g_player.transform), speed * g_window.delta_time));
  }
  if (window_is_keyboard_key_held(KEYBOARD_KEY_A)) {
    velocity = vector3_add(velocity, vector3_muls(transform_local_left(&g_player.transform), speed * g_window.delta_time));
  }
  if (window_is_keyboard_key_held(KEYBOARD_KEY_E)) {
    velocity = vector3_add(velocity, vector3_muls(transform_local_up(&g_player.transform), speed * g_window.delta_time));
  }
  if (window_is_keyboard_key_held(KEYBOARD_KEY_Q)) {
    velocity = vector3_add(velocity, vector3_muls(transform_local_down(&g_player.transform), speed * g_window.delta_time));
  }
  if (window_is_keyboard_key_held(KEYBOARD_KEY_W)) {
    velocity = vector3_add(velocity, vector3_muls(transform_local_front(&g_player.transform), speed * g_window.delta_time));
  }
  if (window_is_keyboard_key_held(KEYBOARD_KEY_S)) {
    velocity = vector3_add(velocity, vector3_muls(transform_local_back(&g_player.transform), speed * g_window.delta_time));
  }

  g_player.linear_velocity = velocity;
}
static void player_handle_rotation(void) {
  float mouse_position_x = (float)g_window.mouse_position_x;
  float mouse_position_y = (float)g_window.mouse_position_y;

  if (window_is_mouse_key_pressed(MOUSE_KEY_RIGHT)) {

    g_player.mouse_begin = vector3_xyz(mouse_position_x, mouse_position_y, 0.0F);
    g_player.mouse_delta = vector3_zero();
  }

  if (window_is_mouse_key_held(MOUSE_KEY_RIGHT)) {

    vector3_t velocity = g_player.angular_velocity;

    g_player.mouse_end = vector3_xyz(mouse_position_x, mouse_position_y, 0.0F);
    g_player.mouse_delta = vector3_sub(g_player.mouse_end, g_player.mouse_begin);
    g_player.mouse_begin = g_player.mouse_end;

    float p_delta = g_player.mouse_delta.y * g_player.mouse_rotation_speed;
    float y_delta = g_player.mouse_delta.x * g_player.mouse_rotation_speed;

    velocity.x += p_delta;
    velocity.y += y_delta;

    g_player.angular_velocity = velocity;
  }
}

static void player_handle_linear_velocity(void) {
  float linear_damping = expf(-g_player.linear_drag * g_window.delta_time);

  g_player.transform.local_position = vector3_add(g_player.transform.local_position, vector3_muls(g_player.linear_velocity, g_window.delta_time));
  g_player.linear_velocity = vector3_muls(g_player.linear_velocity, linear_damping);
}
static void player_handle_angular_velocity(void) {
  float angular_damping = expf(-g_player.angular_drag * g_window.delta_time);

  vector3_t velocity = g_player.angular_velocity;

  quaternion_t qp = quaternion_angle_axis(velocity.x * g_window.delta_time, transform_local_right(&g_player.transform));
  quaternion_t qy = quaternion_angle_axis(velocity.y * g_window.delta_time, vector3_up());
  quaternion_t q = quaternion_mul(qy, qp);

  g_player.transform.local_rotation = quaternion_norm(quaternion_mul(q, g_player.transform.local_rotation));
  g_player.angular_velocity = vector3_muls(g_player.angular_velocity, angular_damping);
}
