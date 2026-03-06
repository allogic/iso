#include <pch.h>

#define CHUNKMGR_CHUNK_RADIUS_X (1)
#define CHUNKMGR_CHUNK_RADIUS_Y (1)
#define CHUNKMGR_CHUNK_RADIUS_Z (1)

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

static chunkmap_t s_curr_active_position = {
  .capacity = CHUNKMGR_CHUNK_RADIUS_X * CHUNKMGR_CHUNK_RADIUS_Y * CHUNKMGR_CHUNK_RADIUS_Z * 16, // TODO: properly scale chunkmap..
};
static chunkmap_t s_prev_active_position = {
  .capacity = CHUNKMGR_CHUNK_RADIUS_X * CHUNKMGR_CHUNK_RADIUS_Y * CHUNKMGR_CHUNK_RADIUS_Z * 16, // TODO: properly scale chunkmap..
};

static chunkmap_t *s_curr_active_position_ptr = &s_curr_active_position;
static chunkmap_t *s_prev_active_position_ptr = &s_prev_active_position;

static uint32_t s_max_chunk_count = 0;
static uint32_t s_store_chunk_count = 0;
static uint32_t s_load_chunk_count = 0;
static uint32_t s_active_chunk_count = 0;

static chunkpool_t s_chunkpool = {0};
static chunktbl_t s_chunktbl = {0};

chunkmgr_t g_chunkmgr = {0};

void chunkmgr_create(void) {
  chunkmap_create(&s_curr_active_position);
  chunkmap_create(&s_prev_active_position);

  s_max_chunk_count = chunkmgr_count_position_in_ellipsoid(CHUNKMGR_CHUNK_RADIUS_X, CHUNKMGR_CHUNK_RADIUS_Y, CHUNKMGR_CHUNK_RADIUS_Z) * 2; // TODO: fix this times two..

  chunkpool_create(&s_chunkpool, s_max_chunk_count);
  chunktbl_create(&s_chunktbl, s_max_chunk_count);

  chunkmgr_create_command_pool();
  chunkmgr_create_command_buffer();

  svdb_create(s_max_chunk_count);
  // dvdb_create(); // TODO
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
  // dvdb_destroy(); // TODO
  svdb_destroy();

  chunkmgr_destroy_command_buffer();
  chunkmgr_destroy_command_pool();

  chunktbl_destroy(&s_chunktbl);
  chunkpool_destroy(&s_chunkpool);

  chunkmap_destroy(&s_curr_active_position);
  chunkmap_destroy(&s_prev_active_position);
}

DWORD WINAPI chunkmgr_worker(LPVOID param) {
  while (g_chunkmgr.is_running) {

    if (g_chunkmgr.async_state == CHUNKMGR_ASYNC_STATE_IDLE) {

      if (g_chunkmgr.build_state == CHUNKMGR_BUILD_STATE_READY) {

        g_chunkmgr.prev_chunk_position = g_chunkmgr.curr_chunk_position;
        g_chunkmgr.curr_chunk_position = (ivector3_t){
          floor_div32(g_player.transform.world_position.x),
          floor_div32(g_player.transform.world_position.y),
          floor_div32(g_player.transform.world_position.z),
        };

        if (ivector3_equal(g_chunkmgr.prev_chunk_position, g_chunkmgr.curr_chunk_position) == 0) {

          chunkmap_t *tmp = s_prev_active_position_ptr;
          s_prev_active_position_ptr = s_curr_active_position_ptr;
          s_curr_active_position_ptr = tmp;

          chunkmap_clear(s_curr_active_position_ptr);

          chunkmgr_collect_position_in_ellipsoid(CHUNKMGR_CHUNK_RADIUS_X, CHUNKMGR_CHUNK_RADIUS_Y, CHUNKMGR_CHUNK_RADIUS_Z);

          s_store_chunk_count = 0;
          s_load_chunk_count = 0;

          VkCommandBufferInheritanceInfo command_buffer_inheritance_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO};

          VkCommandBufferBeginInfo command_buffer_begin_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = &command_buffer_inheritance_info,
          };

          VK_CHECK(vkResetCommandBuffer(g_chunkmgr.command_buffer, 0));
          VK_CHECK(vkBeginCommandBuffer(g_chunkmgr.command_buffer, &command_buffer_begin_info));

          chunkmap_iterate(s_prev_active_position_ptr, chunkmgr_find_store_chunk);
          chunkmap_iterate(s_curr_active_position_ptr, chunkmgr_find_load_chunk);

          svdb_generate_idraw(g_chunkmgr.command_buffer);

          VK_CHECK(vkEndCommandBuffer(g_chunkmgr.command_buffer));

          s_active_chunk_count = s_chunktbl.dense_count;

          ASSERT(s_store_chunk_count < s_max_chunk_count);
          ASSERT(s_load_chunk_count < s_max_chunk_count);
          ASSERT(s_active_chunk_count < s_max_chunk_count);

          chunkmap_dump(s_curr_active_position_ptr);
          chunktbl_dump(&s_chunktbl);

          g_chunkmgr.async_state = CHUNKMGR_ASYNC_STATE_READY;
          g_chunkmgr.build_state = CHUNKMGR_BUILD_STATE_DIRTY;
        }
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

          uint32_t *prev_handle = chunkmap_lookup(s_prev_active_position_ptr, key);

          if (prev_handle) {
            chunkmap_insert(s_curr_active_position_ptr, key, *prev_handle);
          } else {
            uint32_t handle = chunkpool_alloc(&s_chunkpool);

            chunktbl_insert(&s_chunktbl, handle);
            chunkmap_insert(s_curr_active_position_ptr, key, handle);
          }
        }
      }
    }
  }
}

static void chunkmgr_find_store_chunk(ivector3_t key, uint32_t handle) {
  if (chunkmap_lookup(s_curr_active_position_ptr, key) == 0) {

    uint32_t index = chunktbl_lookup(&s_chunktbl, handle);

    // TODO: Handle chunk store..

    chunktbl_remove(&s_chunktbl, handle);

    chunkpool_free(&s_chunkpool, handle);

    s_store_chunk_count++;
  }
}
static void chunkmgr_find_load_chunk(ivector3_t key, uint32_t handle) {
  if (chunkmap_lookup(s_prev_active_position_ptr, key) == 0) {

    uint32_t index = chunktbl_lookup(&s_chunktbl, handle);

    svdb_generate_world(g_chunkmgr.command_buffer, index);
    svdb_generate_mask(g_chunkmgr.command_buffer, index);
    svdb_generate_mesh(g_chunkmgr.command_buffer, index);

    s_load_chunk_count++;
  }
}

static void chunkmgr_destroy_command_pool(void) {
  vkDestroyCommandPool(g_window.device, g_chunkmgr.command_pool, 0);
}
static void chunkmgr_destroy_command_buffer(void) {
  vkFreeCommandBuffers(g_window.device, g_chunkmgr.command_pool, 1, &g_chunkmgr.command_buffer);
}
