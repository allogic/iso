#include <pch.h>

#define CHUNKMGR_CHUNK_RADIUS_X (8)
#define CHUNKMGR_CHUNK_RADIUS_Y (4)
#define CHUNKMGR_CHUNK_RADIUS_Z (8)

DWORD WINAPI chunkmgr_worker(LPVOID param);

static void chunkmgr_create_command_pool(void);
static void chunkmgr_create_command_buffer(void);

static uint32_t chunkmgr_count_position_in_ellipsoid(int32_t radius_x, int32_t radius_y, int32_t radius_z);
static void chunkmgr_collect_position_in_ellipsoid(int32_t radius_x, int32_t radius_y, int32_t radius_z);

static void chunkmgr_find_store_chunk(ivector3_t key, uint32_t handle);
static void chunkmgr_find_load_chunk(ivector3_t key, uint32_t handle);

static void chunkmgr_destroy_command_pool(void);
static void chunkmgr_destroy_command_buffer(void);

static HANDLE s_worker_thread_handle = 0;

static map_t s_curr_active_position = {
  .capacity = CHUNKMGR_CHUNK_RADIUS_X * CHUNKMGR_CHUNK_RADIUS_Y * CHUNKMGR_CHUNK_RADIUS_Z * 16,
};
static map_t s_prev_active_position = {
  .capacity = CHUNKMGR_CHUNK_RADIUS_X * CHUNKMGR_CHUNK_RADIUS_Y * CHUNKMGR_CHUNK_RADIUS_Z * 16,
};

static map_t *s_curr_active_position_ptr = &s_curr_active_position;
static map_t *s_prev_active_position_ptr = &s_prev_active_position;

static uint32_t s_store_chunk_count = 0;
static uint32_t s_load_chunk_count = 0;

static chunkpool_t s_chunkpool = {0};

chunkmgr_t g_chunkmgr = {0};

void chunkmgr_create(void) {
  chunkmgr_create_command_pool();
  chunkmgr_create_command_buffer();

  map_create(&s_curr_active_position);
  map_create(&s_prev_active_position);

  uint32_t chunk_count = chunkmgr_count_position_in_ellipsoid(CHUNKMGR_CHUNK_RADIUS_X, CHUNKMGR_CHUNK_RADIUS_Y, CHUNKMGR_CHUNK_RADIUS_Z) * 2; // TODO: fix this times two..

  s_chunkpool.capacity = chunk_count;
  s_chunkpool.free_count = chunk_count;

  chunkpool_create(&s_chunkpool);
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
  chunkpool_destroy(&s_chunkpool);

  map_destroy(&s_curr_active_position);
  map_destroy(&s_prev_active_position);

  chunkmgr_destroy_command_buffer();
  chunkmgr_destroy_command_pool();
}

DWORD WINAPI chunkmgr_worker(LPVOID param) {
  while (g_chunkmgr.is_running) {

    if (g_chunkmgr.state == CHUNKMGR_STATE_IDLE) {

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

        chunkmgr_collect_position_in_ellipsoid(CHUNKMGR_CHUNK_RADIUS_X, CHUNKMGR_CHUNK_RADIUS_Y, CHUNKMGR_CHUNK_RADIUS_Z);

        g_chunkmgr.state = CHUNKMGR_STATE_RECORDING;

        VkCommandBufferInheritanceInfo command_buffer_inheritance_info = {
          .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO};

        VkCommandBufferBeginInfo command_buffer_begin_info = {
          .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
          .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
          .pInheritanceInfo = &command_buffer_inheritance_info,
        };

        VK_CHECK(vkResetCommandBuffer(g_chunkmgr.command_buffer, 0));
        VK_CHECK(vkBeginCommandBuffer(g_chunkmgr.command_buffer, &command_buffer_begin_info));

        s_store_chunk_count = 0;
        s_load_chunk_count = 0;

        map_iterate(s_prev_active_position_ptr, chunkmgr_find_store_chunk);
        map_iterate(s_curr_active_position_ptr, chunkmgr_find_load_chunk);

        printf("Store: %u Load: %u\n", s_store_chunk_count, s_load_chunk_count);

        VK_CHECK(vkEndCommandBuffer(g_chunkmgr.command_buffer));

        g_chunkmgr.state = CHUNKMGR_STATE_READY;
      }
    }
  }

  return 0;
}

static void chunkmgr_create_command_pool(void) {
  VkCommandPoolCreateInfo command_pool_create_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    .queueFamilyIndex = g_window.primary_queue_index,
  };

  VK_CHECK(vkCreateCommandPool(g_window.device, &command_pool_create_info, 0, &g_chunkmgr.command_pool));
}
static void chunkmgr_create_command_buffer(void) {
  VkCommandBufferAllocateInfo command_buffer_allocate_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .commandPool = g_chunkmgr.command_pool,
    .level = VK_COMMAND_BUFFER_LEVEL_SECONDARY,
    .commandBufferCount = 1,
  };

  VK_CHECK(vkAllocateCommandBuffers(g_window.device, &command_buffer_allocate_info, &g_chunkmgr.command_buffer));
}

static uint32_t chunkmgr_count_position_in_ellipsoid(int32_t radius_x, int32_t radius_y, int32_t radius_z) {
  uint32_t chunk_count = 0;

  int32_t rx2 = radius_x * radius_x;
  int32_t ry2 = radius_y * radius_y;
  int32_t rz2 = radius_z * radius_z;

  for (int32_t dz = -radius_z; dz <= radius_z; dz++) {
    for (int32_t dy = -radius_y; dy <= radius_y; dy++) {
      for (int32_t dx = -radius_x; dx <= radius_x; dx++) {

        if ((dx * dx) * ry2 * rz2 + (dy * dy) * rx2 * rz2 + (dz * dz) * rx2 * ry2 <= rx2 * ry2 * rz2) {

          chunk_count++;
        }
      }
    }
  }

  return chunk_count;
}
static void chunkmgr_collect_position_in_ellipsoid(int32_t radius_x, int32_t radius_y, int32_t radius_z) {
  int32_t rx2 = radius_x * radius_x;
  int32_t ry2 = radius_y * radius_y;
  int32_t rz2 = radius_z * radius_z;

  for (int32_t dz = -radius_z; dz <= radius_z; dz++) {
    for (int32_t dy = -radius_y; dy <= radius_y; dy++) {
      for (int32_t dx = -radius_x; dx <= radius_x; dx++) {

        if ((dx * dx) * ry2 * rz2 + (dy * dy) * rx2 * rz2 + (dz * dz) * rx2 * ry2 <= rx2 * ry2 * rz2) {

          ivector3_t key = ivector3_add(g_chunkmgr.curr_chunk_position, ivector3_xyz(dx, dy, dz)); // TODO: add chunk_position..?

          uint32_t *prev_handle = map_lookup(s_prev_active_position_ptr, key);

          if (prev_handle) {
            map_insert(s_curr_active_position_ptr, key, *prev_handle);
          } else {
            map_insert(s_curr_active_position_ptr, key, chunkpool_alloc(&s_chunkpool));
          }
        }
      }
    }
  }
}

static void chunkmgr_find_store_chunk(ivector3_t key, uint32_t handle) {
  if (map_lookup(s_curr_active_position_ptr, key) == 0) {

    chunk_t *chunk = chunkpool_lookup(&s_chunkpool, handle);

    // TODO: Handle chunk store..

    chunkpool_free(&s_chunkpool, handle);

    s_store_chunk_count++;
  }
}
static void chunkmgr_find_load_chunk(ivector3_t key, uint32_t handle) {
  if (map_lookup(s_prev_active_position_ptr, key) == 0) {

    chunk_t *chunk = chunkpool_lookup(&s_chunkpool, handle);

    // TODO: Handle chunk load..

    s_load_chunk_count++;
  }
}

static void chunkmgr_destroy_command_pool(void) {
  vkDestroyCommandPool(g_window.device, g_chunkmgr.command_pool, 0);
}
static void chunkmgr_destroy_command_buffer(void) {
  vkFreeCommandBuffers(g_window.device, g_chunkmgr.command_pool, 1, &g_chunkmgr.command_buffer);
}
