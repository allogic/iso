#include <pch.h>

#define STRUCTURE_LUT_COUNT (0x400)
#define STRUCTURE_OFFSET_COUNT (0x400)

static void structure_add_conveyor_belt(void);

uint32_t *g_structure_lut = 0;
uint32_t *g_structure_offset = 0;

uint32_t g_structure_count = 0;
uint32_t g_structure_size = 0;

void structure_create(void) {
  g_structure_lut = (uint32_t *)HEAP_ALLOC(sizeof(uint32_t) * STRUCTURE_LUT_COUNT, 1, 0);
  g_structure_offset = (uint32_t *)HEAP_ALLOC(sizeof(uint32_t) * STRUCTURE_OFFSET_COUNT, 1, 0);

  structure_add_conveyor_belt();

#ifdef BUILD_DEBUG
  printf("Structure Stats\n");
  printf("  LUT count: %u\n", g_structure_count);
  printf("  LUT size: %u\n", g_structure_size);
  printf("\n");
#endif // BUILD_DEBUG
}
void structure_destroy(void) {
  HEAP_FREE(g_structure_lut);
  HEAP_FREE(g_structure_offset);
}

static void structure_add_conveyor_belt(void) {
  uint32_t *structure = g_structure_lut + g_structure_offset[g_structure_count];
  uint32_t structure_size = 1 + 3 + 4;

  *structure++ = STRUCTURE_TYPE_CONVEYOR_BELT;

  *structure++ = 2; // X axis
  *structure++ = 1; // Y axis
  *structure++ = 2; // Z axis

  *structure++ = 2; // Tile 0,0,0
  *structure++ = 2; // Tile 1,0,0
  *structure++ = 3; // Tile 0,0,1
  *structure++ = 3; // Tile 1,0,1

  g_structure_offset[g_structure_count + 1] = structure_size;
  g_structure_count += 1;
  g_structure_size += structure_size;
}
