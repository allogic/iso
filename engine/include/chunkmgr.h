#ifndef CHUNKMGR_H
#define CHUNKMGR_H

typedef enum chunkmgr_state_t {
  CHUNKMGR_STATE_IDLE = 0,
  CHUNKMGR_STATE_RECORDING,
  CHUNKMGR_STATE_READY,
  CHUNKMGR_STATE_IN_FLIGHT,
} chunkmgr_state_t;

typedef struct chunkmgr_t {
  chunkmgr_state_t state;
  ivector3_t prev_chunk_position;
  ivector3_t curr_chunk_position;
  uint32_t is_running;
  VkCommandPool command_pool;
  VkCommandBuffer command_buffer;
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
