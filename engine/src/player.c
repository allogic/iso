#include <pch.h>

static void player_handle_position(player_t *player);
static void player_handle_rotation(player_t *player);

static void player_handle_linear_velocity(player_t *player);
static void player_handle_angular_velocity(player_t *player);

player_t g_player_0 = {0};

player_t player_create(void) {
  return (player_t){
    .transform = transform_create(0),
    .camera = camera_create(45.0F, 30.0F, -1000.0F, 1000.0F, 0.01F, 1000.0F),
    .mouse_begin = vector3_zero(),
    .mouse_end = vector3_zero(),
    .mouse_delta = vector3_zero(),
    .linear_velocity = vector3_zero(),
    .angular_velocity = vector3_zero(),
    .keyboard_speed_fast = 1000.0F,
    .keyboard_speed_default = 100.0F,
    .mouse_rotation_speed = deg_to_rad(3.0F),
    .linear_drag = 25.0F,
    .angular_drag = 35.0F,
  };
}
void player_update(player_t *player) {
  player_handle_position(player);
  player_handle_rotation(player);

  player_handle_linear_velocity(player);
  player_handle_angular_velocity(player);
}

static void player_handle_position(player_t *player) {
  float delta_time = g_window.delta_time;
  float speed = window_is_keyboard_key_held(KEYBOARD_KEY_LEFT_SHIFT)
                  ? player->keyboard_speed_fast
                  : player->keyboard_speed_default;

  vector3_t velocity = player->linear_velocity;

  if (window_is_keyboard_key_held(KEYBOARD_KEY_D)) {
    velocity = vector3_add(velocity, vector3_muls(transform_local_right(&player->transform), speed * delta_time));
  }
  if (window_is_keyboard_key_held(KEYBOARD_KEY_A)) {
    velocity = vector3_add(velocity, vector3_muls(transform_local_left(&player->transform), speed * delta_time));
  }
  if (window_is_keyboard_key_held(KEYBOARD_KEY_E)) {
    velocity = vector3_add(velocity, vector3_muls(transform_local_up(&player->transform), speed * delta_time));
  }
  if (window_is_keyboard_key_held(KEYBOARD_KEY_Q)) {
    velocity = vector3_add(velocity, vector3_muls(transform_local_down(&player->transform), speed * delta_time));
  }
  if (window_is_keyboard_key_held(KEYBOARD_KEY_W)) {
    velocity = vector3_add(velocity, vector3_muls(transform_local_front(&player->transform), speed * delta_time));
  }
  if (window_is_keyboard_key_held(KEYBOARD_KEY_S)) {
    velocity = vector3_add(velocity, vector3_muls(transform_local_back(&player->transform), speed * delta_time));
  }

  player->linear_velocity = velocity;
}
static void player_handle_rotation(player_t *player) {
  float mouse_position_x = (float)g_window.mouse_position_x;
  float mouse_position_y = (float)g_window.mouse_position_y;

  if (window_is_mouse_key_pressed(MOUSE_KEY_RIGHT)) {

    player->mouse_begin = vector3_xyz(mouse_position_x, mouse_position_y, 0.0F);
    player->mouse_delta = vector3_zero();
  }

  if (window_is_mouse_key_held(MOUSE_KEY_RIGHT)) {

    vector3_t velocity = player->angular_velocity;

    player->mouse_end = vector3_xyz(mouse_position_x, mouse_position_y, 0.0F);
    player->mouse_delta = vector3_sub(player->mouse_end, player->mouse_begin);
    player->mouse_begin = player->mouse_end;

    float p_delta = player->mouse_delta.y * player->mouse_rotation_speed;
    float y_delta = player->mouse_delta.x * player->mouse_rotation_speed;

    velocity.x += p_delta;
    velocity.y += y_delta;

    player->angular_velocity = velocity;
  }
}

static void player_handle_linear_velocity(player_t *player) {
  float delta_time = g_window.delta_time;
  float linear_damping = expf(-player->linear_drag * delta_time);

  player->transform.local_position = vector3_add(player->transform.local_position, vector3_muls(player->linear_velocity, delta_time));
  player->linear_velocity = vector3_muls(player->linear_velocity, linear_damping);
}
static void player_handle_angular_velocity(player_t *player) {
  float delta_time = g_window.delta_time;
  float angular_damping = expf(-player->angular_drag * delta_time);

  vector3_t velocity = player->angular_velocity;

  quaternion_t qp = quaternion_angle_axis(velocity.x * delta_time, transform_local_right(&player->transform));
  quaternion_t qy = quaternion_angle_axis(velocity.y * delta_time, vector3_up());
  quaternion_t q = quaternion_mul(qy, qp);

  player->transform.local_rotation = quaternion_norm(quaternion_mul(q, player->transform.local_rotation));
  player->angular_velocity = vector3_muls(player->angular_velocity, angular_damping);
}
