#ifndef CHUNK_H
#define CHUNK_H

// TODO: find a way to store command buffers in a contiguous linear block..
//       or maybe vkCmdExecuteCommands accepts unrecorded buffers as well..
typedef struct chunk_t {
  VkCommandBuffer command_buffer;
} chunk_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHUNK_H
