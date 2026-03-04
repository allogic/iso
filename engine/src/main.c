#include <pch.h>

// TODO: check out face buffers so vertex cost can be further minimized..
// TODO: refactor uniforms with "_uniform_t" and make structs..

// TODO: add vkGetBufferDeviceAddress to buffer_t directly..

// TODO: Implement hardware raytracing voxel renderer!
// TODO: Implement augmented vertex block descent (AVBD) collision solver!
// TODO: Finally implement sparse textures!

int32_t main(int32_t argc, char **argv) {

  window_create(1920, 1080, "Engine");
  window_run();
  window_destroy();

  HEAP_RESET();

  return 0;
}
