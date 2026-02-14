#include <pch.h>

int32_t main(int32_t argc, char **argv) {

  window_create(1920, 1080, "Example");
  window_run();
  window_destroy();

  HEAP_RESET();

  return 0;
}
