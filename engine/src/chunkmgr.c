#include <pch.h>

#define CHUNKMGR_CHUNK_RADIUS_X (8)
#define CHUNKMGR_CHUNK_RADIUS_Y (4)
#define CHUNKMGR_CHUNK_RADIUS_Z (8)

DWORD WINAPI chunkmgr_worker(LPVOID param);

static void chunkmgr_collect_positions_in_ellipsoid(int32_t radius_x, int32_t radius_y, int32_t radius_z);

static void chunkmgr_find_load_chunks(ivector3_t key, uint32_t value);
static void chunkmgr_find_store_chunks(ivector3_t key, uint32_t value);

static void chunkmgr_draw_load_chunks(ivector3_t key, uint32_t value);
static void chunkmgr_draw_store_chunks(ivector3_t key, uint32_t value);

static HANDLE s_worker_thread_handle = 0;

static map_t s_curr_active_position = {
  .capacity = CHUNKMGR_CHUNK_RADIUS_X * CHUNKMGR_CHUNK_RADIUS_Y * CHUNKMGR_CHUNK_RADIUS_Z * 16,
};
static map_t s_prev_active_position = {
  .capacity = CHUNKMGR_CHUNK_RADIUS_X * CHUNKMGR_CHUNK_RADIUS_Y * CHUNKMGR_CHUNK_RADIUS_Z * 16,
};
static map_t s_load_position = {
  .capacity = CHUNKMGR_CHUNK_RADIUS_X * CHUNKMGR_CHUNK_RADIUS_Y * CHUNKMGR_CHUNK_RADIUS_Z * 16,
};
static map_t s_store_position = {
  .capacity = CHUNKMGR_CHUNK_RADIUS_X * CHUNKMGR_CHUNK_RADIUS_Y * CHUNKMGR_CHUNK_RADIUS_Z * 16,
};

static map_t *s_curr_active_position_ptr = &s_curr_active_position;
static map_t *s_prev_active_position_ptr = &s_prev_active_position;

chunkmgr_t g_chunkmgr = {0};

void chunkmgr_create(void) {
  VkCommandPoolCreateInfo command_pool_create_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    .queueFamilyIndex = g_window.primary_queue_index,
  };

  VK_CHECK(vkCreateCommandPool(g_window.device, &command_pool_create_info, 0, &g_chunkmgr.command_pool));

  VkCommandBufferAllocateInfo command_buffer_allocate_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .commandPool = g_chunkmgr.command_pool,
    .level = VK_COMMAND_BUFFER_LEVEL_SECONDARY,
    .commandBufferCount = 1,
  };

  VK_CHECK(vkAllocateCommandBuffers(g_window.device, &command_buffer_allocate_info, &g_chunkmgr.command_buffer));

  map_create(&s_curr_active_position);
  map_create(&s_prev_active_position);
  map_create(&s_load_position);
  map_create(&s_store_position);
}
void chunkmgr_start(void) {
  g_chunkmgr.is_running = 1;

  s_worker_thread_handle = CreateThread(0, 0, chunkmgr_worker, 0, 0, 0);
}
void chunkmgr_stop(void) {
  g_chunkmgr.is_running = 0;

  WaitForMultipleObjects(1, &s_worker_thread_handle, 1, INFINITE);

  CloseHandle(s_worker_thread_handle);
}
void chunkmgr_destroy(void) {
  map_destroy(&s_curr_active_position);
  map_destroy(&s_prev_active_position);
  map_destroy(&s_load_position);
  map_destroy(&s_store_position);

  vkFreeCommandBuffers(g_window.device, g_chunkmgr.command_pool, 1, &g_chunkmgr.command_buffer);
  vkDestroyCommandPool(g_window.device, g_chunkmgr.command_pool, 0);
}

DWORD WINAPI chunkmgr_worker(LPVOID param) {
  while (g_chunkmgr.is_running) {

    if (g_chunkmgr.state == CHUNKMGR_STATE_IDLE) {

      g_chunkmgr.state = CHUNKMGR_STATE_RECORDING;

      VkCommandBufferInheritanceInfo command_buffer_inheritance_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO};

      VkCommandBufferBeginInfo command_buffer_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = &command_buffer_inheritance_info,
      };

      VK_CHECK(vkBeginCommandBuffer(g_chunkmgr.command_buffer, &command_buffer_begin_info));

      g_chunkmgr.prev_chunk_position = g_chunkmgr.curr_chunk_position;
      g_chunkmgr.curr_chunk_position = (ivector3_t){
        floor_div32(g_player.transform.world_position.x),
        floor_div32(g_player.transform.world_position.y),
        floor_div32(g_player.transform.world_position.z),
      };

      if (ivector3_equal(g_chunkmgr.prev_chunk_position, g_chunkmgr.curr_chunk_position) == 0) {

        map_t *tmp = s_prev_active_position_ptr;
        s_prev_active_position_ptr = s_curr_active_position_ptr;
        s_curr_active_position_ptr = tmp;

        map_clear(s_curr_active_position_ptr);
        map_clear(&s_load_position);
        map_clear(&s_store_position);

        chunkmgr_collect_positions_in_ellipsoid(CHUNKMGR_CHUNK_RADIUS_X, CHUNKMGR_CHUNK_RADIUS_Y, CHUNKMGR_CHUNK_RADIUS_Z);

        g_chunkmgr.chunk_count = s_curr_active_position_ptr->count;

        // map_iterate(s_curr_active_position_ptr, chunkmgr_find_load_chunks);
        // map_iterate(s_prev_active_position_ptr, chunkmgr_find_store_chunks);
      }

      map_iterate(&s_load_position, chunkmgr_draw_load_chunks);
      map_iterate(&s_store_position, chunkmgr_draw_store_chunks);

      VK_CHECK(vkEndCommandBuffer(g_chunkmgr.command_buffer));

      g_chunkmgr.state = CHUNKMGR_STATE_READY;
    }
  }

  return 0;
}

static void chunkmgr_collect_positions_in_ellipsoid(int32_t radius_x, int32_t radius_y, int32_t radius_z) {
  int32_t rx2 = radius_x * radius_x;
  int32_t ry2 = radius_y * radius_y;
  int32_t rz2 = radius_z * radius_z;

  for (int32_t dz = -radius_z; dz <= radius_z; dz++) {
    for (int32_t dy = -radius_y; dy <= radius_y; dy++) {
      for (int32_t dx = -radius_x; dx <= radius_x; dx++) {

        if ((dx * dx) * ry2 * rz2 + (dy * dy) * rx2 * rz2 + (dz * dz) * rx2 * ry2 <= rx2 * ry2 * rz2) {

          ivector3_t chunk_position = ivector3_add(g_chunkmgr.curr_chunk_position, ivector3_xyz(dx, dy, dz));

          map_insert(s_curr_active_position_ptr, chunk_position, 0);
        }
      }
    }
  }
}

static void chunkmgr_find_load_chunks(ivector3_t key, uint32_t value) {
  if (map_lookup(s_prev_active_position_ptr, key) == 0) {
    map_insert(&s_load_position, key, 0);
  }
}
static void chunkmgr_find_store_chunks(ivector3_t key, uint32_t value) {
  if (map_lookup(s_curr_active_position_ptr, key) == 0) {
    map_insert(&s_store_position, key, 0);
  }
}

static void chunkmgr_draw_load_chunks(ivector3_t key, uint32_t value) {
  renderer_draw_debug_box(
    (vector3_t){(float)key.x * SVDB_CHUNK_SIZE, (float)key.y * SVDB_CHUNK_SIZE, (float)key.z * SVDB_CHUNK_SIZE},
    (vector3_t){(float)SVDB_CHUNK_SIZE * 0.99F, (float)SVDB_CHUNK_SIZE * 0.99F, (float)SVDB_CHUNK_SIZE * 0.99F},
    (vector4_t){0.0F, 1.0F, 0.0F, 1.0F});
}
static void chunkmgr_draw_store_chunks(ivector3_t key, uint32_t value) {
  renderer_draw_debug_box(
    (vector3_t){(float)key.x * SVDB_CHUNK_SIZE, (float)key.y * SVDB_CHUNK_SIZE, (float)key.z * SVDB_CHUNK_SIZE},
    (vector3_t){(float)SVDB_CHUNK_SIZE * 0.99F, (float)SVDB_CHUNK_SIZE * 0.99F, (float)SVDB_CHUNK_SIZE * 0.99F},
    (vector4_t){1.0F, 0.0F, 0.0F, 1.0F});
}
