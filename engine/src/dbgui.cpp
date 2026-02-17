#include <pch.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_vulkan.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND window_handle, UINT window_message, WPARAM w_param, LPARAM l_param);

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

  ImGui::Begin("Player Transform");
  ImGui::DragFloat3("Local Position", (float *)&g_player_0.transform.local_position, 0.05F);
  vector3_t euler_angles = quaternion_to_euler_angles(g_player_0.transform.local_rotation);
  euler_angles.x = rad_to_deg(euler_angles.x);
  euler_angles.y = rad_to_deg(euler_angles.y);
  euler_angles.z = rad_to_deg(euler_angles.z);
  ImGui::DragFloat3("Local Rotation", (float *)&euler_angles, 0.05F);
  ImGui::DragFloat3("Local Scale", (float *)&g_player_0.transform.local_scale, 0.05F);
  ImGui::End();

  ImGui::Begin("Player Camera");
  ImGui::Checkbox("Orthographic", (bool *)&g_player_0.camera.is_orthographic);
  ImGui::DragFloat("FoV", &g_player_0.camera.fov_deg, 0.05F);
  ImGui::DragFloat("Zoom", &g_player_0.camera.zoom, 0.05F);
  ImGui::DragFloat("Persp Near Z", &g_player_0.camera.persp_near_z, 0.05F);
  ImGui::DragFloat("Persp Far Z", &g_player_0.camera.persp_far_z, 0.05F);
  ImGui::DragFloat("Ortho Near Z", &g_player_0.camera.ortho_near_z, 0.05F);
  ImGui::DragFloat("Ortho Far Z", &g_player_0.camera.ortho_far_z, 0.05F);
  ImGui::End();

  ImGui::Begin("ISO Mesher");
  ImGui::DragFloat4("Vertex Offset 0", (float *)&g_renderer.vertex_offset_0, 0.05F);
  ImGui::DragFloat4("Vertex Offset 1", (float *)&g_renderer.vertex_offset_1, 0.05F);
  ImGui::DragFloat4("Vertex Offset 2", (float *)&g_renderer.vertex_offset_2, 0.05F);
  ImGui::DragFloat4("Vertex Offset 3", (float *)&g_renderer.vertex_offset_3, 0.05F);
  ImGui::DragFloat4("Rotation", (float *)&g_renderer.rotation, 0.05F);
  ImGui::DragFloat("Tile Size", &g_renderer.tile_size, 0.05F);
  ImGui::DragFloat("Tile Height", &g_renderer.tile_height, 0.05F);
  ImGui::DragFloat("Inner Scale", &g_renderer.inner_scale, 0.05F);
  ImGui::DragFloat("Outer Epsilon", &g_renderer.outer_scale, 0.05F);
  ImGui::End();

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
