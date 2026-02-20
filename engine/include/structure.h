#ifndef STRUCTURE_H
#define STRUCTURE_H

typedef enum structure_type_t {
  STRUCTURE_TYPE_CONVEYOR_BELT = 0,
} structure_type_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern uint32_t *g_structure_lut;
extern uint32_t *g_structure_offset;

extern uint32_t g_structure_count;

void structure_create(void);
void structure_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // STRUCTURE_H
