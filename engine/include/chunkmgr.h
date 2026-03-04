#ifndef CHUNKMGR_H
#define CHUNKMGR_H

typedef struct chunkmgr_t {
  ivector3_t prev_chunk_position;
  ivector3_t curr_chunk_position;
  uint32_t is_running;
  uint32_t chunk_count;
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
