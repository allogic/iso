#include <pch.h>

#define VENDOR_ID_NVIDIA (0x10DE)
#define VENDOR_ID_AMD (0x1002)
#define VENDOR_ID_INTEL (0x8086)

#define MAX_SURFACE_FORMATS (0xFF)
#define MAX_PRESENT_MODES (0xFF)

static LRESULT window_native_message_proc(HWND window_handle, UINT window_message, WPARAM w_param, LPARAM l_param);

#ifdef BUILD_DEBUG
static VkBool32 window_vulkan_message_proc(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, VkDebugUtilsMessengerCallbackDataEXT const *callback_data, void *user_data);
#endif // BUILD_DEBUG

static void window_create_native(void);
static void window_create_instance(void);
static void window_create_surface(void);
static void window_create_device(void);
static void window_create_command_pool(void);
static void window_create_command_buffer(void);

static void window_find_physical_device(void);
static void window_find_physical_device_queue_families(void);

static void window_check_physical_device_extensions(void);
static void window_check_physical_device_features(void);

static void window_update_surface_capabilities(void);

static void window_destroy_native(void);
static void window_destroy_instance(void);
static void window_destroy_surface(void);
static void window_destroy_device(void);
static void window_destroy_command_pool(void);
static void window_destroy_command_buffer(void);

static char const *s_window_class = "ENGINE_WND_CLASS";

#ifdef BUILD_DEBUG
static char const *s_enabled_layer[] = {
  "VK_LAYER_KHRONOS_validation",
};
#endif // BUILD_DEBUG

static char const *s_instance_extension[] = {
  "VK_KHR_surface",
  "VK_KHR_win32_surface",
#ifdef BUILD_DEBUG
  "VK_EXT_debug_utils",
#endif // BUILD_DEBUG
};

static char const *s_device_extension[] = {
  "VK_KHR_swapchain",
  "VK_KHR_fragment_shading_rate", // TODO: remove me..
  "VK_KHR_ray_tracing_pipeline",
  "VK_KHR_acceleration_structure",
  "VK_KHR_deferred_host_operations",
  "VK_EXT_mesh_shader", // TODO: do i really need task/mesh shaders..
};

VkPhysicalDeviceRayTracingPipelinePropertiesKHR g_physical_device_ray_tracing_pipeline_properties = {
  .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR,
  .pNext = 0,
};

VkPhysicalDeviceRayTracingPipelineFeaturesKHR g_physical_device_ray_tracing_pipeline_features = {
  .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR,
  .pNext = 0,
};
VkPhysicalDeviceAccelerationStructureFeaturesKHR g_physical_device_acceleration_structure_features = {
  .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR,
  .pNext = &g_physical_device_ray_tracing_pipeline_features,
};
VkPhysicalDevice8BitStorageFeatures g_physical_device_8bit_storage_features = {
  .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES,
  .pNext = &g_physical_device_acceleration_structure_features,
};
VkPhysicalDeviceBufferDeviceAddressFeatures g_physical_device_buffer_device_address_freatures = {
  .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES,
  .pNext = &g_physical_device_8bit_storage_features,
};
VkPhysicalDeviceVulkanMemoryModelFeatures g_physical_device_vulkan_memory_model_features = {
  .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES,
  .pNext = &g_physical_device_buffer_device_address_freatures,
};
VkPhysicalDeviceTimelineSemaphoreFeatures g_physical_device_timeline_semaphore_features = {
  .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES,
  .pNext = &g_physical_device_vulkan_memory_model_features,
};
VkPhysicalDeviceMaintenance4Features g_physical_device_maintenance4_features = {
  .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES,
  .pNext = &g_physical_device_timeline_semaphore_features,
};
VkPhysicalDeviceFragmentShadingRateFeaturesKHR g_physical_device_fragment_shading_rate_features = {
  .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR,
  .pNext = &g_physical_device_maintenance4_features,
};
VkPhysicalDeviceMultiviewFeatures g_physical_device_multiview_features = {
  .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES,
  .pNext = &g_physical_device_fragment_shading_rate_features,
};
VkPhysicalDeviceMeshShaderFeaturesEXT g_physical_device_mesh_shader_features = {
  .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT,
  .pNext = &g_physical_device_multiview_features,
};
VkPhysicalDeviceDescriptorIndexingFeatures g_physical_device_descriptor_indexing_features = {
  .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
  .pNext = &g_physical_device_mesh_shader_features,
};

window_t g_window = {0};

#ifdef BUILD_DEBUG
PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT_proc = 0;
PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT_proc = 0;
#endif // BUILD_DEBUG

PFN_vkCmdDrawMeshTasksEXT vkCmdDrawMeshTasksEXT_proc = 0;
PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR_proc = 0;
PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR_proc = 0;

PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR_proc = 0;
PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR_proc = 0;

PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR_proc = 0;
PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR_proc = 0;
PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR_proc = 0;

PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR_proc = 0;

void window_create(uint32_t width, uint32_t height, char const *title) {
  g_window.window_width = width;
  g_window.window_height = height;
  g_window.window_title = title;
  g_window.is_first_frame = 1;

  window_create_native();
  window_create_instance();
  window_create_surface();

  window_find_physical_device();
  window_find_physical_device_queue_families();

  window_check_physical_device_extensions();
  window_check_physical_device_features();

  window_create_device();
  window_create_command_pool();
  window_create_command_buffer();

  window_update_surface_capabilities();

  structure_create();
  static_vdb_create();
  dynamic_vdb_create();
  renderpass_create();
  swapchain_create();
  framebuffer_create();
  renderer_create();
  dbgui_create();
}
void window_run(void) {
  QueryPerformanceFrequency(&g_window.time_freq);
  QueryPerformanceCounter(&g_window.time_prev);

  while (g_window.is_window_running) {

    g_window.mouse_wheel_delta = 0;

    uint32_t keyboard_key_index = 0;
    uint32_t keyboard_key_count = KEYBOARD_KEY_COUNT;

    while (keyboard_key_index < keyboard_key_count) {

      if (g_window.keyboard_key_states[keyboard_key_index] == KEY_STATE_PRESSED) {
        g_window.keyboard_key_states[keyboard_key_index] = KEY_STATE_DOWN;
      } else if (g_window.keyboard_key_states[keyboard_key_index] == KEY_STATE_RELEASED) {
        g_window.keyboard_key_states[keyboard_key_index] = KEY_STATE_UP;
      }

      keyboard_key_index++;
    }

    uint32_t mouse_key_index = 0;
    uint32_t mouse_key_count = MOUSE_KEY_COUNT;

    while (mouse_key_index < mouse_key_count) {

      if (g_window.mouse_key_states[mouse_key_index] == KEY_STATE_PRESSED) {
        g_window.mouse_key_states[mouse_key_index] = KEY_STATE_DOWN;
      } else if (g_window.mouse_key_states[mouse_key_index] == KEY_STATE_RELEASED) {
        g_window.mouse_key_states[mouse_key_index] = KEY_STATE_UP;
      }

      mouse_key_index++;
    }

    if (g_swapchain.is_dirty) {

      g_swapchain.is_dirty = 0;

      VK_CHECK(vkQueueWaitIdle(g_window.primary_queue));
      VK_CHECK(vkQueueWaitIdle(g_window.present_queue));

      framebuffer_destroy();
      swapchain_destroy();

      window_update_surface_capabilities();

      swapchain_create();
      framebuffer_create();

      renderer_update();
    }

    while (PeekMessageA(&g_window.window_message, 0, 0, 0, PM_REMOVE)) {

      TranslateMessage(&g_window.window_message);
      DispatchMessageA(&g_window.window_message);
    }

    player_update();

    static_vdb_debug();
    dynamic_vdb_debug();

    renderer_draw();

    QueryPerformanceCounter(&g_window.time_curr);

    double time_freq = (double)g_window.time_freq.QuadPart;
    double time_prev = (double)g_window.time_prev.QuadPart;
    double time_curr = (double)g_window.time_curr.QuadPart;

    float delta_time = (float)((time_curr - time_prev) / time_freq);

    delta_time = clampf(delta_time, 0.0F, WINDOW_MAX_DELTA_TIME);

    g_window.delta_time = delta_time;
    g_window.time_prev = g_window.time_curr;

    g_window.time += delta_time;
    g_window.elapsed_time_since_fps_count_update += delta_time;

    g_window.fps_counter++;
    g_window.frame_index++;

    if ((g_window.elapsed_time_since_fps_count_update > 1.0F) || (g_window.is_first_frame)) {

      static char title_buffer[0x400] = {0};

      snprintf(title_buffer, sizeof(title_buffer), "%s %s.%s.%s (%s) - %d FPS",
               g_window.window_title,
               VERSION_MAJOR,
               VERSION_MINOR,
               VERSION_PATCH,
               GIT_VERSION_HASH,
               g_window.fps_counter);

      SetWindowTextA(g_window.window_handle, title_buffer);

      g_window.elapsed_time_since_fps_count_update = 0.0F;
      g_window.fps_counter = 0;
    }

    g_window.is_first_frame = 0;
  }
}
void window_destroy(void) {
  VK_CHECK(vkQueueWaitIdle(g_window.primary_queue));
  VK_CHECK(vkQueueWaitIdle(g_window.present_queue));

  dbgui_destroy();
  renderer_destroy();
  framebuffer_destroy();
  swapchain_destroy();
  renderpass_destroy();
  dynamic_vdb_destroy();
  static_vdb_destroy();
  structure_destroy();

  window_destroy_command_buffer();
  window_destroy_command_pool();
  window_destroy_device();
  window_destroy_surface();
  window_destroy_instance();
  window_destroy_native();
}

uint32_t window_is_keyboard_key_pressed(keyboard_key_t key) {
  return g_window.keyboard_key_states[key] == KEY_STATE_PRESSED;
}
uint32_t window_is_keyboard_key_held(keyboard_key_t key) {
  return (g_window.keyboard_key_states[key] == KEY_STATE_DOWN) || (g_window.keyboard_key_states[key] == KEY_STATE_PRESSED);
}
uint32_t window_is_keyboard_key_released(keyboard_key_t key) {
  return g_window.keyboard_key_states[key] == KEY_STATE_RELEASED;
}

uint32_t window_is_mouse_key_pressed(mouse_key_t key) {
  return g_window.mouse_key_states[key] == KEY_STATE_PRESSED;
}
uint32_t window_is_mouse_key_held(mouse_key_t key) {
  return (g_window.mouse_key_states[key] == KEY_STATE_DOWN) || (g_window.mouse_key_states[key] == KEY_STATE_PRESSED);
}
uint32_t window_is_mouse_key_released(mouse_key_t key) {
  return g_window.mouse_key_states[key] == KEY_STATE_RELEASED;
}

static LRESULT window_native_message_proc(HWND window_handle, UINT window_message, WPARAM w_param, LPARAM l_param) {
  window_t *window = (window_t *)GetWindowLongPtr(window_handle, GWLP_USERDATA);

  dbgui_message(window_handle, window_message, w_param, l_param);

  switch (window_message) {

    case WM_CREATE: {

      window->is_window_running = 1;

      break;
    }
    case WM_CLOSE: {

      window->is_window_running = 0;

      break;
    }

    case WM_NCCREATE: {

      SetWindowLongPtr(window_handle, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT *)l_param)->lpCreateParams);

      return TRUE;
    }
    case WM_NCDESTROY: {

      SetWindowLongPtr(window_handle, GWLP_USERDATA, 0);

      break;
    }

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN: {

      UINT scan_code = MapVirtualKeyA((UINT)w_param, MAPVK_VK_TO_VSC);
      UINT virtual_key = MapVirtualKeyExA(scan_code, MAPVK_VSC_TO_VK_EX, GetKeyboardLayout(0));

      switch (virtual_key) {
        case KEYBOARD_KEY_LEFT_SHIFT:
          window->keyboard_key_states[KEYBOARD_KEY_LEFT_SHIFT] = ((window->keyboard_key_states[KEYBOARD_KEY_LEFT_SHIFT] == KEY_STATE_UP) || (window->keyboard_key_states[KEYBOARD_KEY_LEFT_SHIFT] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
        case KEYBOARD_KEY_RIGHT_SHIFT:
          window->keyboard_key_states[KEYBOARD_KEY_RIGHT_SHIFT] = ((window->keyboard_key_states[KEYBOARD_KEY_RIGHT_SHIFT] == KEY_STATE_UP) || (window->keyboard_key_states[KEYBOARD_KEY_RIGHT_SHIFT] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
        case KEYBOARD_KEY_LEFT_CONTROL:
          window->keyboard_key_states[KEYBOARD_KEY_LEFT_CONTROL] = ((window->keyboard_key_states[KEYBOARD_KEY_LEFT_CONTROL] == KEY_STATE_UP) || (window->keyboard_key_states[KEYBOARD_KEY_LEFT_CONTROL] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
        case KEYBOARD_KEY_RIGHT_CONTROL:
          window->keyboard_key_states[KEYBOARD_KEY_RIGHT_CONTROL] = ((window->keyboard_key_states[KEYBOARD_KEY_RIGHT_CONTROL] == KEY_STATE_UP) || (window->keyboard_key_states[KEYBOARD_KEY_RIGHT_CONTROL] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
        case KEYBOARD_KEY_LEFT_MENU:
          window->keyboard_key_states[KEYBOARD_KEY_LEFT_MENU] = ((window->keyboard_key_states[KEYBOARD_KEY_LEFT_MENU] == KEY_STATE_UP) || (window->keyboard_key_states[KEYBOARD_KEY_LEFT_MENU] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
        case KEYBOARD_KEY_RIGHT_MENU:
          window->keyboard_key_states[KEYBOARD_KEY_RIGHT_MENU] = ((window->keyboard_key_states[KEYBOARD_KEY_RIGHT_MENU] == KEY_STATE_UP) || (window->keyboard_key_states[KEYBOARD_KEY_RIGHT_MENU] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
        default:
          window->keyboard_key_states[virtual_key] = ((window->keyboard_key_states[virtual_key] == KEY_STATE_UP) || (window->keyboard_key_states[virtual_key] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
      }

      break;
    }

    case WM_KEYUP:
    case WM_SYSKEYUP: {

      UINT scan_code = MapVirtualKeyA((UINT)w_param, MAPVK_VK_TO_VSC);
      UINT virtual_key = MapVirtualKeyExA(scan_code, MAPVK_VSC_TO_VK_EX, GetKeyboardLayout(0));

      switch (virtual_key) {
        case KEYBOARD_KEY_LEFT_SHIFT:
          window->keyboard_key_states[KEYBOARD_KEY_LEFT_SHIFT] = ((window->keyboard_key_states[KEYBOARD_KEY_LEFT_SHIFT] == KEY_STATE_DOWN) || (window->keyboard_key_states[KEYBOARD_KEY_LEFT_SHIFT] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
        case KEYBOARD_KEY_RIGHT_SHIFT:
          window->keyboard_key_states[KEYBOARD_KEY_RIGHT_SHIFT] = ((window->keyboard_key_states[KEYBOARD_KEY_RIGHT_SHIFT] == KEY_STATE_DOWN) || (window->keyboard_key_states[KEYBOARD_KEY_RIGHT_SHIFT] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
        case KEYBOARD_KEY_LEFT_CONTROL:
          window->keyboard_key_states[KEYBOARD_KEY_LEFT_CONTROL] = ((window->keyboard_key_states[KEYBOARD_KEY_LEFT_CONTROL] == KEY_STATE_DOWN) || (window->keyboard_key_states[KEYBOARD_KEY_LEFT_CONTROL] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
        case KEYBOARD_KEY_RIGHT_CONTROL:
          window->keyboard_key_states[KEYBOARD_KEY_RIGHT_CONTROL] = ((window->keyboard_key_states[KEYBOARD_KEY_RIGHT_CONTROL] == KEY_STATE_DOWN) || (window->keyboard_key_states[KEYBOARD_KEY_RIGHT_CONTROL] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
        case KEYBOARD_KEY_LEFT_MENU:
          window->keyboard_key_states[KEYBOARD_KEY_LEFT_MENU] = ((window->keyboard_key_states[KEYBOARD_KEY_LEFT_MENU] == KEY_STATE_DOWN) || (window->keyboard_key_states[KEYBOARD_KEY_LEFT_MENU] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
        case KEYBOARD_KEY_RIGHT_MENU:
          window->keyboard_key_states[KEYBOARD_KEY_RIGHT_MENU] = ((window->keyboard_key_states[KEYBOARD_KEY_RIGHT_MENU] == KEY_STATE_DOWN) || (window->keyboard_key_states[KEYBOARD_KEY_RIGHT_MENU] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
        default:
          window->keyboard_key_states[virtual_key] = ((window->keyboard_key_states[virtual_key] == KEY_STATE_DOWN) || (window->keyboard_key_states[virtual_key] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
      }

      break;
    }

    case WM_LBUTTONDOWN: {

      window->mouse_key_states[MOUSE_KEY_LEFT] = ((window->mouse_key_states[MOUSE_KEY_LEFT] == KEY_STATE_UP) || (window->mouse_key_states[MOUSE_KEY_LEFT] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;

      break;
    }
    case WM_LBUTTONUP: {

      window->mouse_key_states[MOUSE_KEY_LEFT] = ((window->mouse_key_states[MOUSE_KEY_LEFT] == KEY_STATE_DOWN) || (window->mouse_key_states[MOUSE_KEY_LEFT] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;

      break;
    }
    case WM_MBUTTONDOWN: {

      window->mouse_key_states[MOUSE_KEY_MIDDLE] = ((window->mouse_key_states[MOUSE_KEY_MIDDLE] == KEY_STATE_UP) || (window->mouse_key_states[MOUSE_KEY_MIDDLE] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;

      break;
    }
    case WM_MBUTTONUP: {

      window->mouse_key_states[MOUSE_KEY_MIDDLE] = ((window->mouse_key_states[MOUSE_KEY_MIDDLE] == KEY_STATE_DOWN) || (window->mouse_key_states[MOUSE_KEY_MIDDLE] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;

      break;
    }
    case WM_RBUTTONDOWN: {

      window->mouse_key_states[MOUSE_KEY_RIGHT] = ((window->mouse_key_states[MOUSE_KEY_RIGHT] == KEY_STATE_UP) || (window->mouse_key_states[MOUSE_KEY_RIGHT] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;

      break;
    }
    case WM_RBUTTONUP: {

      window->mouse_key_states[MOUSE_KEY_RIGHT] = ((window->mouse_key_states[MOUSE_KEY_RIGHT] == KEY_STATE_DOWN) || (window->mouse_key_states[MOUSE_KEY_RIGHT] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;

      break;
    }
    case WM_LBUTTONDBLCLK: {

      break;
    }
    case WM_MBUTTONDBLCLK: {

      break;
    }
    case WM_RBUTTONDBLCLK: {

      break;
    }

    case WM_MOUSEMOVE: {

      INT mouse_x = LOWORD(l_param);
      INT mouse_y = HIWORD(l_param);

      window->mouse_position_x = mouse_x;
      window->mouse_position_y = mouse_y;

      break;
    }
    case WM_MOUSEWHEEL: {

      window->mouse_wheel_delta = GET_WHEEL_DELTA_WPARAM(w_param) / WHEEL_DELTA;

      break;
    }

    default: {

      return DefWindowProcA(window_handle, window_message, w_param, l_param);
    }
  }

  return 1;
}

#ifdef BUILD_DEBUG
static VkBool32 window_vulkan_message_proc(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, VkDebugUtilsMessengerCallbackDataEXT const *callback_data, void *user_data) {
  printf("%s\n", callback_data->pMessage);

  __debugbreak();

  return 0;
}
#endif // BUILD_DEBUG

static void window_create_native(void) {
  g_window.module_handle = GetModuleHandleA(0);

  WNDCLASSEX window_class_ex = {
    .cbSize = sizeof(WNDCLASSEX),
    .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
    .lpfnWndProc = window_native_message_proc,
    .cbClsExtra = 0,
    .cbWndExtra = 0,
    .hInstance = g_window.module_handle,
    .hIcon = LoadIconA(0, IDI_APPLICATION),
    .hCursor = LoadCursorA(0, IDC_ARROW),
    .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
    .lpszMenuName = 0,
    .lpszClassName = s_window_class,
    .hIconSm = LoadIconA(0, IDI_APPLICATION),
  };

  RegisterClassExA(&window_class_ex);

  INT screen_width = GetSystemMetrics(SM_CXSCREEN);
  INT screen_height = GetSystemMetrics(SM_CYSCREEN);
  INT window_position_x = (screen_width - g_window.window_width) / 2;
  INT window_position_y = (screen_height - g_window.window_height) / 2;

  g_window.window_handle = CreateWindowExA(
    0,
    s_window_class, g_window.window_title,
    WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
    window_position_x, window_position_y,
    g_window.window_width, g_window.window_height,
    0,
    0,
    g_window.module_handle,
    &g_window);

  ShowWindow(g_window.window_handle, SW_SHOW);
  UpdateWindow(g_window.window_handle);
}
static void window_create_instance(void) {
  uint32_t vulkan_api_version = VK_API_VERSION_1_3;

  VkApplicationInfo application_info = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pApplicationName = "VULKAN_APPLICATION",
    .applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
    .pEngineName = "VULKAN_ENGINE",
    .engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
    .apiVersion = vulkan_api_version,
  };

#ifdef BUILD_DEBUG
  VkValidationFeatureEnableEXT validation_feature_enable[] = {
    VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
    VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
    VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT,
  };

  VkValidationFeaturesEXT validation_features = {
    .sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
    .pNext = 0,
    .enabledValidationFeatureCount = 0, // ARRAY_COUNT(validation_feature_enable),
    .pEnabledValidationFeatures = 0,    // validation_feature_enable,
    .disabledValidationFeatureCount = 0,
    .pDisabledValidationFeatures = 0,
  };

  VkDebugUtilsMessengerCreateInfoEXT debug_utils_messenger_create_info = {
    .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
    .pNext = 0,
    .flags = 0,
    .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
    .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
    .pfnUserCallback = window_vulkan_message_proc,
    .pUserData = 0,
  };
#endif // BUILD_DEBUG

  VkInstanceCreateInfo instance_create_info = {
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pApplicationInfo = &application_info,
    .ppEnabledExtensionNames = s_instance_extension,
    .enabledExtensionCount = ARRAY_COUNT(s_instance_extension),
#ifdef BUILD_DEBUG
    .pNext = &debug_utils_messenger_create_info,
    .ppEnabledLayerNames = s_enabled_layer,
    .enabledLayerCount = ARRAY_COUNT(s_enabled_layer),
#endif // BUILD_DEBUG
  };

  VK_CHECK(vkCreateInstance(&instance_create_info, 0, &g_window.instance));

#ifdef BUILD_DEBUG
  vkCreateDebugUtilsMessengerEXT_proc = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(g_window.instance, "vkCreateDebugUtilsMessengerEXT");
  vkDestroyDebugUtilsMessengerEXT_proc = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(g_window.instance, "vkDestroyDebugUtilsMessengerEXT");

  VK_CHECK(vkCreateDebugUtilsMessengerEXT_proc(g_window.instance, &debug_utils_messenger_create_info, 0, &g_window.debug_utils_messenger));

  uint32_t instance_version = 0;

  VkResult result = vkEnumerateInstanceVersion(&instance_version);

  if (result == VK_SUCCESS) {

    uint32_t major = VK_VERSION_MAJOR(instance_version);
    uint32_t minor = VK_VERSION_MINOR(instance_version);
    uint32_t patch = VK_VERSION_PATCH(instance_version);

    printf("Vulkan Runtime Version: %d.%d.%d\n", major, minor, patch);

  } else {

    printf("vkEnumerateInstanceVersion not supported, default to 1.0\n");
  }

  printf("Vulkan Header Version: %d\n", VK_HEADER_VERSION);
  printf("\n");
#endif // BUILD_DEBUG
}
static void window_create_surface(void) {
  VkWin32SurfaceCreateInfoKHR win32_surface_create_info = {
    .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
    .hwnd = g_window.window_handle,
    .hinstance = g_window.module_handle,
  };

  VK_CHECK(vkCreateWin32SurfaceKHR(g_window.instance, &win32_surface_create_info, 0, &g_window.surface));
}
static void window_create_device(void) {
  float queue_priority = 1.0F;

  VkDeviceQueueCreateInfo device_queue_create_infos[2] = {
    {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .queueFamilyIndex = g_window.primary_queue_index,
      .queueCount = 1,
      .pQueuePriorities = &queue_priority,
    },
    {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .queueFamilyIndex = g_window.present_queue_index,
      .queueCount = 1,
      .pQueuePriorities = &queue_priority,
    },
  };

  VkDeviceCreateInfo device_create_info = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .pNext = &g_window.physical_device_features2,
    .pQueueCreateInfos = device_queue_create_infos,
    .queueCreateInfoCount = ARRAY_COUNT(device_queue_create_infos),
    .pEnabledFeatures = 0,
    .ppEnabledExtensionNames = s_device_extension,
    .enabledExtensionCount = ARRAY_COUNT(s_device_extension),
#ifdef BUILD_DEBUG
    .ppEnabledLayerNames = s_enabled_layer,
    .enabledLayerCount = ARRAY_COUNT(s_enabled_layer),
#endif // BUILD_DEBUG
  };

  VK_CHECK(vkCreateDevice(g_window.physical_device, &device_create_info, 0, &g_window.device));

  vkGetDeviceQueue(g_window.device, g_window.primary_queue_index, 0, &g_window.primary_queue);
  vkGetDeviceQueue(g_window.device, g_window.present_queue_index, 0, &g_window.present_queue);

  vkCmdDrawMeshTasksEXT_proc = (PFN_vkCmdDrawMeshTasksEXT)vkGetDeviceProcAddr(g_window.device, "vkCmdDrawMeshTasksEXT");
  vkCmdTraceRaysKHR_proc = (PFN_vkCmdTraceRaysKHR)vkGetDeviceProcAddr(g_window.device, "vkCmdTraceRaysKHR");
  vkCmdBuildAccelerationStructuresKHR_proc = (PFN_vkCmdBuildAccelerationStructuresKHR)vkGetDeviceProcAddr(g_window.device, "vkCmdBuildAccelerationStructuresKHR");

  vkCreateAccelerationStructureKHR_proc = (PFN_vkCreateAccelerationStructureKHR)vkGetDeviceProcAddr(g_window.device, "vkCreateAccelerationStructureKHR");
  vkCreateRayTracingPipelinesKHR_proc = (PFN_vkCreateRayTracingPipelinesKHR)vkGetDeviceProcAddr(g_window.device, "vkCreateRayTracingPipelinesKHR");

  vkGetAccelerationStructureBuildSizesKHR_proc = (PFN_vkGetAccelerationStructureBuildSizesKHR)vkGetDeviceProcAddr(g_window.device, "vkGetAccelerationStructureBuildSizesKHR");
  vkGetAccelerationStructureDeviceAddressKHR_proc = (PFN_vkGetAccelerationStructureDeviceAddressKHR)vkGetDeviceProcAddr(g_window.device, "vkGetAccelerationStructureDeviceAddressKHR");
  vkGetRayTracingShaderGroupHandlesKHR_proc = (PFN_vkGetRayTracingShaderGroupHandlesKHR)vkGetDeviceProcAddr(g_window.device, "vkGetRayTracingShaderGroupHandlesKHR");

  vkDestroyAccelerationStructureKHR_proc = (PFN_vkDestroyAccelerationStructureKHR)vkGetDeviceProcAddr(g_window.device, "vkDestroyAccelerationStructureKHR");
}
static void window_create_command_pool(void) {
  VkCommandPoolCreateInfo command_pool_create_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    .queueFamilyIndex = g_window.primary_queue_index,
  };

  VK_CHECK(vkCreateCommandPool(g_window.device, &command_pool_create_info, 0, &g_window.command_pool));
}
static void window_create_command_buffer(void) {
  VkCommandBufferAllocateInfo command_buffer_allocate_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .commandPool = g_window.command_pool,
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = 1,
  };

  VK_CHECK(vkAllocateCommandBuffers(g_window.device, &command_buffer_allocate_info, &g_window.command_buffer));
}

static void window_find_physical_device(void) {
  uint32_t physical_device_index = 0;
  uint32_t physical_device_count = 0;

  static VkPhysicalDevice physical_devices[WINDOW_MAX_PHYSICAL_DEVICES] = {0};

  VK_CHECK(vkEnumeratePhysicalDevices(g_window.instance, &physical_device_count, 0));
  VK_CHECK(vkEnumeratePhysicalDevices(g_window.instance, &physical_device_count, physical_devices));

  while (physical_device_index < physical_device_count) {

    VkPhysicalDevice physical_device = physical_devices[physical_device_index];

    g_window.physical_device_features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    g_window.physical_device_features2.pNext = &g_physical_device_descriptor_indexing_features;

    g_window.physical_device_properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    g_window.physical_device_properties2.pNext = &g_physical_device_ray_tracing_pipeline_properties;

    g_window.physical_device_memory_properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
    g_window.physical_device_memory_properties2.pNext = 0;

    vkGetPhysicalDeviceFeatures2(physical_device, &g_window.physical_device_features2);
    vkGetPhysicalDeviceProperties2(physical_device, &g_window.physical_device_properties2);
    vkGetPhysicalDeviceMemoryProperties2(physical_device, &g_window.physical_device_memory_properties2);

    if (g_window.physical_device_properties2.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {

      g_window.physical_device = physical_device;

      break;
    }

    physical_device_index++;
  }

#ifdef BUILD_DEBUG
  VkPhysicalDeviceProperties *props = &g_window.physical_device_properties2.properties;

  char const *vendor_name = 0;

  switch (props->vendorID) {
    case VENDOR_ID_NVIDIA:
      vendor_name = "NVIDIA";
      break;
    case VENDOR_ID_AMD:
      vendor_name = "AMD";
      break;
    case VENDOR_ID_INTEL:
      vendor_name = "INTEL";
      break;
    default:
      vendor_name = "UNKNOWN";
      break;
  }

  printf("Selected Physical Device\n");
  printf("  Device Index: %d\n", physical_device_index);
  printf("  Device Name: %s\n", props->deviceName);
  printf("  Vulkan Version: %u.%u.%u\n", VK_VERSION_MAJOR(props->apiVersion), VK_VERSION_MINOR(props->apiVersion), VK_VERSION_PATCH(props->apiVersion));
  switch (props->vendorID) {
    case VENDOR_ID_NVIDIA:
      printf("  Driver Version: %u.%u.%u\n", (props->driverVersion >> 22) & 0x3FF, (props->driverVersion >> 14) & 0xFF, (props->driverVersion >> 0) & 0x3FFF);
      break;
    case VENDOR_ID_AMD:
      printf("  Driver Version: %u.%u.%u\n", (props->driverVersion >> 22) & 0x3FF, (props->driverVersion >> 12) & 0x3FF, (props->driverVersion >> 0) & 0xFFF);
      break;
    case VENDOR_ID_INTEL:
      printf("  Driver Version: %u\n", props->driverVersion);
      break;
    default:
      printf("Driver Version: %u\n", props->driverVersion);
      break;
  }
  printf("  Vendor Name: %s\n", vendor_name);
  printf("  Vendor ID: 0x%X\n", props->vendorID);
  printf("  Device ID: 0x%X\n", props->deviceID);
  printf("  Device Type: %d\n", props->deviceType);
  printf("\n");
#endif // BUILD_DEBUG
}
static void window_find_physical_device_queue_families(void) {
  int32_t primary_queue_index = -1;
  int32_t present_queue_index = -1;

  uint32_t queue_family_property_index = 0;
  uint32_t queue_family_property_count = 0;

  static VkQueueFamilyProperties queue_family_properties[WINDOW_MAX_QUEUE_FAMILY_PROPERTIES_COUNT] = {0};

  vkGetPhysicalDeviceQueueFamilyProperties(g_window.physical_device, &queue_family_property_count, 0);
  vkGetPhysicalDeviceQueueFamilyProperties(g_window.physical_device, &queue_family_property_count, queue_family_properties);

  while (queue_family_property_index < queue_family_property_count) {

    VkQueueFamilyProperties queue_family_property = queue_family_properties[queue_family_property_index];

    VkBool32 graphics_support = queue_family_property.queueFlags & VK_QUEUE_GRAPHICS_BIT;
    VkBool32 compute_support = queue_family_property.queueFlags & VK_QUEUE_COMPUTE_BIT;
    VkBool32 present_support = 0;

    VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(g_window.physical_device, (uint32_t)queue_family_property_index, g_window.surface, &present_support));

    if (graphics_support && compute_support && (primary_queue_index == -1)) {

      primary_queue_index = queue_family_property_index;

    } else if (present_support && (present_queue_index == -1)) {

      present_queue_index = queue_family_property_index;
    }

    if ((primary_queue_index != -1) && (present_queue_index != -1)) {

      g_window.primary_queue_index = (uint32_t)primary_queue_index;
      g_window.present_queue_index = (uint32_t)present_queue_index;

      break;
    }

    queue_family_property_index++;
  }

#ifdef BUILD_DEBUG
  printf("Selected Physical Queues\n");
  printf("  Primary Queue Index: %d\n", g_window.primary_queue_index);
  printf("  Present Queue Index: %d\n", g_window.present_queue_index);
  printf("\n");
#endif // BUILD_DEBUG
}

static void window_check_physical_device_extensions(void) {
  uint32_t available_device_extension_count = 0;

  static VkExtensionProperties available_extension_properties[WINDOW_MAX_EXTENSION_PROPERTIES_COUNT] = {0};

  VK_CHECK(vkEnumerateDeviceExtensionProperties(g_window.physical_device, 0, &available_device_extension_count, 0));
  VK_CHECK(vkEnumerateDeviceExtensionProperties(g_window.physical_device, 0, &available_device_extension_count, available_extension_properties));

#ifdef BUILD_DEBUG
  printf("Required Device Extensions\n");
#endif // BUILD_DEBUG

  uint32_t device_extension_index = 0;
  uint32_t device_extension_count = ARRAY_COUNT(s_device_extension);

  while (device_extension_index < device_extension_count) {

    uint32_t device_extensions_available = 0;

    uint32_t available_device_extension_index = 0;

    while (available_device_extension_index < available_device_extension_count) {

      if (strcmp(s_device_extension[device_extension_index], available_extension_properties[available_device_extension_index].extensionName) == 0) {

#ifdef BUILD_DEBUG
        printf("  %s: 1\n", s_device_extension[device_extension_index]);
#endif // BUILD_DEBUG

        device_extensions_available = 1;

        break;
      }

      available_device_extension_index++;
    }

    if (device_extensions_available == 0) {

#ifdef BUILD_DEBUG
      printf("  %s: 0\n", s_device_extension[device_extension_index]);
#endif // BUILD_DEBUG

      break;
    }

    device_extension_index++;
  }

#ifdef BUILD_DEBUG
  printf("\n");
#endif // BUILD_DEBUG
}
static void window_check_physical_device_features(void) {
#ifdef BUILD_DEBUG
  printf("Required Device Features\n");
  printf("  VkPhysicalDeviceMeshShaderFeaturesEXT::taskShader: %d\n", g_physical_device_mesh_shader_features.taskShader);
  printf("  VkPhysicalDeviceMeshShaderFeaturesEXT::meshShader: %d\n", g_physical_device_mesh_shader_features.meshShader);
  printf("  VkPhysicalDeviceDescriptorIndexingFeatures::runtimeDescriptorArray: %d\n", g_physical_device_descriptor_indexing_features.runtimeDescriptorArray);
  printf("  VkPhysicalDeviceBufferDeviceAddressFeatures::bufferDeviceAddress: %d\n", g_physical_device_buffer_device_address_freatures.bufferDeviceAddress);
  printf("  VkPhysicalDeviceRayTracingPipelineFeaturesKHR::rayTracingPipeline: %d\n", g_physical_device_ray_tracing_pipeline_features.rayTracingPipeline);
  printf("  VkPhysicalDeviceAccelerationStructureFeaturesKHR::accelerationStructure: %d\n", g_physical_device_acceleration_structure_features.accelerationStructure);
  printf("\n");
#endif // BUILD_DEBUG
}

static void window_update_surface_capabilities(void) {
  VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(g_window.physical_device, g_window.surface, &g_window.surface_capabilities));

  g_window.window_width = g_window.surface_capabilities.currentExtent.width;
  g_window.window_height = g_window.surface_capabilities.currentExtent.height;
  g_window.min_image_count = g_window.surface_capabilities.minImageCount;
  g_window.max_image_count = g_window.surface_capabilities.maxImageCount;
  g_window.surface_transform = g_window.surface_capabilities.currentTransform;
}

static void window_destroy_native(void) {
  DestroyWindow(g_window.window_handle);

  UnregisterClassA(s_window_class, g_window.module_handle);
}
static void window_destroy_instance(void) {
#ifdef BUILD_DEBUG
  vkDestroyDebugUtilsMessengerEXT_proc(g_window.instance, g_window.debug_utils_messenger, 0);
#endif // BUILD_DEBUG

  vkDestroyInstance(g_window.instance, 0);
}
static void window_destroy_surface(void) {
  vkDestroySurfaceKHR(g_window.instance, g_window.surface, 0);
}
static void window_destroy_device(void) {
  vkDestroyDevice(g_window.device, 0);
}
static void window_destroy_command_pool(void) {
  vkDestroyCommandPool(g_window.device, g_window.command_pool, 0);
}
static void window_destroy_command_buffer(void) {
  vkFreeCommandBuffers(g_window.device, g_window.command_pool, 1, &g_window.command_buffer);
}
