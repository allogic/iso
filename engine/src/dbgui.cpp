#include <pch.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_vulkan.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND window_handle, UINT window_message, WPARAM w_param, LPARAM l_param);

static void dbgui_draw_vdb_mask_generator(void);
static void dbgui_draw_vdb_geom_renderer(void);

static void dbgui_draw_cellular_noise(vdb_terrain_layer_t *layer);
static void dbgui_draw_curl_noise(vdb_terrain_layer_t *layer);

static VkDescriptorPool s_dbgui_descriptor_pool = 0;

static VkDescriptorPoolSize s_dbgui_descriptor_pool_sizes[] = {
  {
    .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    .descriptorCount = 1,
  },
};

void dbgui_create(void) {
  VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
    .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
    .maxSets = 1,
    .poolSizeCount = ARRAY_COUNT(s_dbgui_descriptor_pool_sizes),
    .pPoolSizes = s_dbgui_descriptor_pool_sizes,
  };

  vkCreateDescriptorPool(g_window.device, &descriptor_pool_create_info, 0, &s_dbgui_descriptor_pool);

  IMGUI_CHECKVERSION();

  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  ImGuiPlatformIO &platform_io = ImGui::GetPlatformIO();
  ImGuiStyle &style = ImGui::GetStyle();

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigWindowsMoveFromTitleBarOnly = 1;

  ImGui_ImplWin32_Init(g_window.window_handle);

  ImGui_ImplVulkan_InitInfo imgui_vulkan_init_info = {0};
  imgui_vulkan_init_info.Instance = g_window.instance;
  imgui_vulkan_init_info.PhysicalDevice = g_window.physical_device;
  imgui_vulkan_init_info.Device = g_window.device;
  imgui_vulkan_init_info.QueueFamily = g_window.primary_queue_index;
  imgui_vulkan_init_info.Queue = g_window.primary_queue;
  imgui_vulkan_init_info.PipelineCache = 0;
  imgui_vulkan_init_info.DescriptorPool = s_dbgui_descriptor_pool;
  imgui_vulkan_init_info.PipelineInfoMain.RenderPass = g_renderpass_main;
  imgui_vulkan_init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  imgui_vulkan_init_info.MinImageCount = g_window.min_image_count;
  imgui_vulkan_init_info.ImageCount = g_swapchain.image_count;
  imgui_vulkan_init_info.Allocator = 0;
  imgui_vulkan_init_info.CheckVkResultFn = 0;

  ImGui_ImplVulkan_Init(&imgui_vulkan_init_info);
}
void dbgui_draw(void) {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplWin32_NewFrame();

  ImGui::NewFrame();

  dbgui_draw_vdb_mask_generator();
  dbgui_draw_vdb_geom_renderer();

  ImGui::Render();

  ImDrawData *draw_data = ImGui::GetDrawData();

  ImGui_ImplVulkan_RenderDrawData(draw_data, g_window.command_buffer);
}
void dbgui_message(HWND window_handle, UINT window_message, WPARAM w_param, LPARAM l_param) {
  ImGui_ImplWin32_WndProcHandler(window_handle, window_message, w_param, l_param);
}
void dbgui_destroy(void) {
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplWin32_Shutdown();

  ImGui::DestroyContext();

  vkDestroyDescriptorPool(g_window.device, s_dbgui_descriptor_pool, 0);
}

static void dbgui_draw_vdb_mask_generator(void) {
  ImGui::Begin("Terrain Layer");

  int32_t layer_index = 0;
  int32_t layer_count = VDB_TERRAIN_LAYER_COUNT;

  while (layer_index < layer_count) {

    vdb_terrain_layer_t *layer = &((vdb_terrain_layer_t *)g_vdb.terrain_layer_buffer.mapped_memory)[layer_index];

    if (layer) {

      if (ImGui::TreeNodeEx(layer, ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding, "Layer %d", layer_index)) {

        static char const *noise_type[] = {
          "Cellular",
          "Curl",
        };

        if (ImGui::BeginCombo("Noise Type", noise_type[layer->noise_type])) {

          int32_t noise_type_index = 0;
          int32_t noise_type_count = ARRAY_COUNT(noise_type);

          while (noise_type_index < noise_type_count) {

            bool is_selected = (layer->noise_type == noise_type_index);

            if (ImGui::Selectable(noise_type[noise_type_index], is_selected)) {
              layer->noise_type = noise_type_index;

              g_renderer.rebuild_world = 1;
            }

            if (is_selected) {
              ImGui::SetItemDefaultFocus();
            }

            noise_type_index++;
          }

          ImGui::EndCombo();
        }

        switch (layer->noise_type) {
          case VDB_NOISE_TYPE_CELLULAR: {

            dbgui_draw_cellular_noise(layer);

            break;
          }
          case VDB_NOISE_TYPE_CURL: {

            dbgui_draw_curl_noise(layer);

            break;
          }
        }

        ImGui::TreePop();
      }
    }

    layer_index++;
  }

  ImGui::End();
}
static void dbgui_draw_vdb_geom_renderer(void) {
  ImGui::Begin("VDB Mesher");

  ImGui::End();
}

static void dbgui_draw_cellular_noise(vdb_terrain_layer_t *layer) {
  cellular_noise_args_t *args = &layer->cellular_noise_args;

  static char const *types[] = {
    "2D",
    "3D",
  };

  static char const *axis[] = {
    "XY",
    "XZ",
    "YX",
    "YZ",
  };

  if (ImGui::BeginCombo("Type", types[args->type])) {

    int32_t type_index = 0;
    int32_t type_count = ARRAY_COUNT(types);

    while (type_index < type_count) {

      bool is_selected = (args->type == type_index);

      if (ImGui::Selectable(types[type_index], is_selected)) {
        args->type = type_index;

        g_renderer.rebuild_world = 1;
      }

      if (is_selected) {
        ImGui::SetItemDefaultFocus();
      }

      type_index++;
    }

    ImGui::EndCombo();
  }

  switch (args->type) {
    case VDB_CELLULAR_TYPE_0: {

      if (ImGui::BeginCombo("Axis", axis[args->axis])) {

        int32_t axis_index = 0;
        int32_t axis_count = ARRAY_COUNT(axis);

        while (axis_index < axis_count) {

          bool is_selected = (args->axis == axis_index);

          if (ImGui::Selectable(axis[axis_index], is_selected)) {
            args->axis = axis_index;

            g_renderer.rebuild_world = 1;
          }

          if (is_selected) {
            ImGui::SetItemDefaultFocus();
          }

          axis_index++;
        }

        ImGui::EndCombo();
      }

      break;
    }
  }

  if (ImGui::DragFloat4("Offset", (float *)&args->offset, 0.1F, 0.0F, 0.0F, "%.3F")) {
    g_renderer.rebuild_world = 1;
  }

  if (ImGui::DragFloat("U", &args->u, 0.1F, 0.0F, 0.0F, "%.3F")) {
    g_renderer.rebuild_world = 1;
  }

  if (ImGui::DragFloat("V", &args->v, 0.1F, 0.0F, 0.0F, "%.3F")) {
    g_renderer.rebuild_world = 1;
  }

  if (ImGui::DragFloat("Scale", &layer->scale, 0.001F, 0.0F, 0.0F, "%.3F")) {
    g_renderer.rebuild_world = 1;
  }

  if (ImGui::DragFloat("Weight", &layer->weight, 0.01F, 0.0F, 0.0F, "%.3F")) {
    g_renderer.rebuild_world = 1;
  }
}
static void dbgui_draw_curl_noise(vdb_terrain_layer_t *layer) {
  curl_noise_args_t *args = &layer->curl_noise_args;

  static char const *types[] = {
    "2D",
    "3D",
    "4D",
  };

  static char const *axis[] = {
    "XY",
    "XZ",
    "YX",
    "YZ",
  };

  if (ImGui::BeginCombo("Type", types[args->type])) {

    int32_t type_index = 0;
    int32_t type_count = ARRAY_COUNT(types);

    while (type_index < type_count) {

      bool is_selected = (args->type == type_index);

      if (ImGui::Selectable(types[type_index], is_selected)) {
        args->type = type_index;

        g_renderer.rebuild_world = 1;
      }

      if (is_selected) {
        ImGui::SetItemDefaultFocus();
      }

      type_index++;
    }

    ImGui::EndCombo();
  }

  switch (args->type) {
    case VDB_CELLULAR_TYPE_0: {

      if (ImGui::BeginCombo("Axis", axis[args->axis])) {

        int32_t axis_index = 0;
        int32_t axis_count = ARRAY_COUNT(axis);

        while (axis_index < axis_count) {

          bool is_selected = (args->axis == axis_index);

          if (ImGui::Selectable(axis[axis_index], is_selected)) {
            args->axis = axis_index;

            g_renderer.rebuild_world = 1;
          }

          if (is_selected) {
            ImGui::SetItemDefaultFocus();
          }

          axis_index++;
        }

        ImGui::EndCombo();
      }

      break;
    }
  }

  if (ImGui::DragFloat4("Offset", (float *)&args->offset, 0.1F, 0.0F, 0.0F, "%.3F")) {
    g_renderer.rebuild_world = 1;
  }

  if (ImGui::DragFloat("Scale", &layer->scale, 0.001F, 0.0F, 0.0F, "%.3F")) {
    g_renderer.rebuild_world = 1;
  }

  if (ImGui::DragFloat("Weight", &layer->weight, 0.01F, 0.0F, 0.0F, "%.3F")) {
    g_renderer.rebuild_world = 1;
  }
}
