#ifndef CHUNKMGR_H
#define CHUNKMGR_H

typedef enum chunkmgr_async_state_t {
  CHUNKMGR_ASYNC_STATE_IDLE = 0,
  CHUNKMGR_ASYNC_STATE_READY,
  CHUNKMGR_ASYNC_STATE_IN_FLIGHT,
  CHUNKMGR_ASYNC_STATE_COUNT,
} chunkmgr_async_state_t;

typedef enum chunkmgr_build_state_t {
  CHUNKMGR_BUILD_STATE_READY = 0,
  CHUNKMGR_BUILD_STATE_DIRTY,
  CHUNKMGR_BUILD_STATE_COUNT,
} chunkmgr_build_state_t;

typedef struct chunkmgr_t {
  chunkmgr_async_state_t async_state;
  chunkmgr_build_state_t build_state;
  ivector3_t prev_chunk_position;
  ivector3_t curr_chunk_position;
  uint32_t is_running;
  uint32_t chunk_count;
  VkCommandPool command_pool;
  VkCommandBuffer *compute_command_buffer;
  VkCommandBuffer *graphics_command_buffer;
} chunkmgr_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern chunkmgr_t g_chunkmgr;

void chunkmgr_create(void);
void chunkmgr_start(void);
void chunkmgr_stop(void);
void chunkmgr_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHUNKMGR_H
