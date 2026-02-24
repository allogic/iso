#include <pch.h>

//    32 ^ 3 sides        = 32768
// 32768 / 2 checkerboard = 16384
// 16384 x 6 faces        = 98304
// 98304 x 4 vertices     = 393216
// 98304 x 6 indices      = 589824
#define WORST_CASE_GREEDY_MESH_VERTEX_COUNT (400000)
#define WORST_CASE_GREEDY_MESH_INDEX_COUNT (600000)

static void static_vdb_create_chunk_voxel_image(void);

static void static_vdb_create_chunk_info_buffer(void);
static void static_vdb_create_chunk_mask_buffer(void);
static void static_vdb_create_chunk_vertex_buffer(void);
static void static_vdb_create_chunk_index_buffer(void);

static void static_vdb_generate_world(void);
static void static_vdb_generate_mask(void);
static void static_vdb_generate_mesh(void);

static void static_vdb_destroy_image(void);
static void static_vdb_destroy_buffer(void);

static_vdb_t g_static_vdb = {
  .chunk_info_buffer = {
    .size = sizeof(static_vdb_chunk_info_t) * STATIC_VDB_CHUNK_COUNT,
    .buffer_usage_flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
    .memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
  },
  .chunk_mask_buffer = {
    .size = sizeof(static_vdb_chunk_mask_t) * STATIC_VDB_CHUNK_COUNT,
    .buffer_usage_flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
    .memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
  },
};

dynamic_vdb_t g_dynamic_vdb = {0};

void static_vdb_create(void) {
  static_vdb_create_chunk_voxel_image();

  static_vdb_create_chunk_info_buffer();
  static_vdb_create_chunk_mask_buffer();
  static_vdb_create_chunk_vertex_buffer();
  static_vdb_create_chunk_index_buffer();
}
void static_vdb_build(void) {
  static_vdb_generate_world();
  static_vdb_generate_mask();
  static_vdb_generate_mesh();
}
void static_vdb_debug(void) {
  renderer_draw_debug_box(
    (vector3_t){0.0F, 0.0F, 0.0F},
    (vector3_t){(float)STATIC_VDB_CHUNK_SIZE * STATIC_VDB_DIM_X, (float)STATIC_VDB_CHUNK_SIZE * STATIC_VDB_DIM_Y, (float)STATIC_VDB_CHUNK_SIZE * STATIC_VDB_DIM_Z},
    (vector4_t){1.0F, 1.0F, 1.0F, 1.0F});
}
void static_vdb_draw(void) {
  static_vdb_chunk_info_t *chunk_info = (static_vdb_chunk_info_t *)g_static_vdb.chunk_info_buffer.device_data;

  vkCmdBindPipeline(g_window.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_renderer.static_vdb_renderer_pipeline.pipeline_handle);

  static_vdb_renderer_push_constant_t static_vdb_renderer_push_constant = {0};

  int32_t chunk_index = 0;
  int32_t chunk_count = STATIC_VDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    static_vdb_renderer_push_constant.chunk_position = static_vdb_chunk_index_to_position(chunk_index);
    static_vdb_renderer_push_constant.chunk_index = chunk_index;

    VkDeviceSize vertex_offset[] = {0};

    vkCmdBindVertexBuffers(g_window.command_buffer, 0, 1, &g_static_vdb.chunk_vertex_buffer[chunk_index].buffer_handle, vertex_offset);
    vkCmdBindIndexBuffer(g_window.command_buffer, g_static_vdb.chunk_index_buffer[chunk_index].buffer_handle, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(g_window.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_renderer.static_vdb_renderer_pipeline.pipeline_layout, 0, 1, &g_renderer.static_vdb_renderer_pipeline.descriptor_set[chunk_index], 0, 0);
    vkCmdPushConstants(g_window.command_buffer, g_renderer.static_vdb_renderer_pipeline.pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(static_vdb_renderer_push_constant), &static_vdb_renderer_push_constant);
    vkCmdDrawIndexed(g_window.command_buffer, chunk_info[chunk_index].index_count, 1, 0, 0, 0);

    chunk_index++;
  }
}
void static_vdb_destroy(void) {
  static_vdb_destroy_image();
  static_vdb_destroy_buffer();
}

int32_t static_vdb_chunk_position_to_index(ivector3_t chunk_position) {
  return (chunk_position.x) +
         (chunk_position.y * STATIC_VDB_DIM_X) +
         (chunk_position.z * STATIC_VDB_DIM_X * STATIC_VDB_DIM_Y);
}
ivector3_t static_vdb_chunk_index_to_position(int32_t chunk_index) {
  return (ivector3_t){
    chunk_index % STATIC_VDB_DIM_X,
    (chunk_index / STATIC_VDB_DIM_X) % STATIC_VDB_DIM_Y,
    chunk_index / (STATIC_VDB_DIM_X * STATIC_VDB_DIM_Y),
  };
}

void dynamic_vdb_create(void) {
}
void dynamic_vdb_draw(void) {
}
void dynamic_vdb_debug(void) {
}
void dynamic_vdb_destroy(void) {
}

static void static_vdb_create_chunk_voxel_image(void) {
  g_static_vdb.chunk_voxel_image = (image_t *)HEAP_ALLOC(sizeof(image_t) * STATIC_VDB_CHUNK_COUNT, 1, 0);
  g_static_vdb.chunk_voxel_descriptor_image_info = (VkDescriptorImageInfo *)HEAP_ALLOC(sizeof(VkDescriptorImageInfo) * STATIC_VDB_CHUNK_COUNT, 1, 0);

  int32_t chunk_index = 0;
  int32_t chunk_count = STATIC_VDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    g_static_vdb.chunk_voxel_image[chunk_index].width = STATIC_VDB_CHUNK_SIZE,
    g_static_vdb.chunk_voxel_image[chunk_index].height = STATIC_VDB_CHUNK_SIZE,
    g_static_vdb.chunk_voxel_image[chunk_index].depth = STATIC_VDB_CHUNK_SIZE,
    g_static_vdb.chunk_voxel_image[chunk_index].channel = 1,
    g_static_vdb.chunk_voxel_image[chunk_index].element_size = sizeof(uint32_t),
    g_static_vdb.chunk_voxel_image[chunk_index].format = VK_FORMAT_R32_UINT,
    g_static_vdb.chunk_voxel_image[chunk_index].filter = VK_FILTER_NEAREST,
    g_static_vdb.chunk_voxel_image[chunk_index].image_usage_flags = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
    g_static_vdb.chunk_voxel_image[chunk_index].image_type = VK_IMAGE_TYPE_3D,
    g_static_vdb.chunk_voxel_image[chunk_index].image_view_type = VK_IMAGE_VIEW_TYPE_3D,
    g_static_vdb.chunk_voxel_image[chunk_index].image_aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT,
    g_static_vdb.chunk_voxel_image[chunk_index].image_tiling = VK_IMAGE_TILING_OPTIMAL,

    image_create(&g_static_vdb.chunk_voxel_image[chunk_index]);

    g_static_vdb.chunk_voxel_descriptor_image_info[chunk_index].sampler = g_static_vdb.chunk_voxel_image[chunk_index].sampler;
    g_static_vdb.chunk_voxel_descriptor_image_info[chunk_index].imageView = g_static_vdb.chunk_voxel_image[chunk_index].image_view;
    g_static_vdb.chunk_voxel_descriptor_image_info[chunk_index].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    chunk_index++;
  }
}

static void static_vdb_create_chunk_info_buffer(void) {
  buffer_create(&g_static_vdb.chunk_info_buffer);

  buffer_map(&g_static_vdb.chunk_info_buffer);

  g_static_vdb.chunk_info_descriptor_buffer_info.offset = 0;
  g_static_vdb.chunk_info_descriptor_buffer_info.buffer = g_static_vdb.chunk_info_buffer.buffer_handle;
  g_static_vdb.chunk_info_descriptor_buffer_info.range = VK_WHOLE_SIZE;
}
static void static_vdb_create_chunk_mask_buffer(void) {
  buffer_create(&g_static_vdb.chunk_mask_buffer);

  g_static_vdb.chunk_mask_descriptor_buffer_info.offset = 0;
  g_static_vdb.chunk_mask_descriptor_buffer_info.buffer = g_static_vdb.chunk_mask_buffer.buffer_handle;
  g_static_vdb.chunk_mask_descriptor_buffer_info.range = VK_WHOLE_SIZE;
}
static void static_vdb_create_chunk_vertex_buffer(void) {
  g_static_vdb.chunk_vertex_buffer = (buffer_t *)HEAP_ALLOC(sizeof(buffer_t) * STATIC_VDB_CHUNK_COUNT, 1, 0);
  g_static_vdb.chunk_vertex_descriptor_buffer_info = (VkDescriptorBufferInfo *)HEAP_ALLOC(sizeof(VkDescriptorBufferInfo) * STATIC_VDB_CHUNK_COUNT, 1, 0);

  int32_t chunk_index = 0;
  int32_t chunk_count = STATIC_VDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    g_static_vdb.chunk_vertex_buffer[chunk_index].size = sizeof(static_vdb_chunk_vertex_t) * WORST_CASE_GREEDY_MESH_VERTEX_COUNT;
    g_static_vdb.chunk_vertex_buffer[chunk_index].buffer_usage_flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
    g_static_vdb.chunk_vertex_buffer[chunk_index].memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,

    buffer_create(&g_static_vdb.chunk_vertex_buffer[chunk_index]);

    g_static_vdb.chunk_vertex_descriptor_buffer_info[chunk_index].offset = 0;
    g_static_vdb.chunk_vertex_descriptor_buffer_info[chunk_index].buffer = g_static_vdb.chunk_vertex_buffer[chunk_index].buffer_handle;
    g_static_vdb.chunk_vertex_descriptor_buffer_info[chunk_index].range = VK_WHOLE_SIZE;

    chunk_index++;
  }
}
static void static_vdb_create_chunk_index_buffer(void) {
  g_static_vdb.chunk_index_buffer = (buffer_t *)HEAP_ALLOC(sizeof(buffer_t) * STATIC_VDB_CHUNK_COUNT, 1, 0);
  g_static_vdb.chunk_index_descriptor_buffer_info = (VkDescriptorBufferInfo *)HEAP_ALLOC(sizeof(VkDescriptorBufferInfo) * STATIC_VDB_CHUNK_COUNT, 1, 0);

  int32_t chunk_index = 0;
  int32_t chunk_count = STATIC_VDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    g_static_vdb.chunk_index_buffer[chunk_index].size = sizeof(static_vdb_chunk_index_t) * WORST_CASE_GREEDY_MESH_INDEX_COUNT;
    g_static_vdb.chunk_index_buffer[chunk_index].buffer_usage_flags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
    g_static_vdb.chunk_index_buffer[chunk_index].memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,

    buffer_create(&g_static_vdb.chunk_index_buffer[chunk_index]);

    g_static_vdb.chunk_index_descriptor_buffer_info[chunk_index].offset = 0;
    g_static_vdb.chunk_index_descriptor_buffer_info[chunk_index].buffer = g_static_vdb.chunk_index_buffer[chunk_index].buffer_handle;
    g_static_vdb.chunk_index_descriptor_buffer_info[chunk_index].range = VK_WHOLE_SIZE;

    chunk_index++;
  }
}

static void static_vdb_generate_world(void) {
  vkCmdBindPipeline(g_window.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, g_renderer.static_vdb_world_generator_pipeline.pipeline_handle);
  vkCmdBindDescriptorSets(g_window.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, g_renderer.static_vdb_world_generator_pipeline.pipeline_layout, 0, 1, &g_renderer.static_vdb_world_generator_pipeline.descriptor_set[0], 0, 0);

  static_vdb_world_generator_push_constant_t static_vdb_world_generator_push_constant = {0};

  int32_t chunk_index = 0;
  int32_t chunk_count = STATIC_VDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    static_vdb_world_generator_push_constant.chunk_position = static_vdb_chunk_index_to_position(chunk_index);
    static_vdb_world_generator_push_constant.chunk_index = chunk_index;
    static_vdb_world_generator_push_constant.stage = 0;

    vkCmdPushConstants(g_window.command_buffer, g_renderer.static_vdb_world_generator_pipeline.pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(static_vdb_world_generator_push_constant), &static_vdb_world_generator_push_constant);
    vkCmdDispatch(g_window.command_buffer, 4, 4, 4);

    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
      .newLayout = VK_IMAGE_LAYOUT_GENERAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = g_static_vdb.chunk_voxel_image[chunk_index].image_handle,
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
      .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
    };

    vkCmdPipelineBarrier(
      g_window.command_buffer,
      VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
      VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
      0,
      0,
      0,
      0,
      0,
      1,
      &image_memory_barrier);

    chunk_index++;
  }

  chunk_index = 0;

  while (chunk_index < chunk_count) {

    static_vdb_world_generator_push_constant.chunk_position = static_vdb_chunk_index_to_position(chunk_index);
    static_vdb_world_generator_push_constant.chunk_index = chunk_index;
    static_vdb_world_generator_push_constant.stage = 1;

    vkCmdPushConstants(g_window.command_buffer, g_renderer.static_vdb_world_generator_pipeline.pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(static_vdb_world_generator_push_constant), &static_vdb_world_generator_push_constant);
    vkCmdDispatch(g_window.command_buffer, 4, 4, 4);

    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
      .newLayout = VK_IMAGE_LAYOUT_GENERAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = g_static_vdb.chunk_voxel_image[chunk_index].image_handle,
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
      .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
    };

    vkCmdPipelineBarrier(
      g_window.command_buffer,
      VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
      VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
      0,
      0,
      0,
      0,
      0,
      1,
      &image_memory_barrier);

    chunk_index++;
  }
}
static void static_vdb_generate_mask(void) {
  vkCmdBindPipeline(g_window.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, g_renderer.static_vdb_mask_generator_pipeline.pipeline_handle);
  vkCmdBindDescriptorSets(g_window.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, g_renderer.static_vdb_mask_generator_pipeline.pipeline_layout, 0, 1, &g_renderer.static_vdb_mask_generator_pipeline.descriptor_set[0], 0, 0);

  static_vdb_mask_generator_push_constant_t static_vdb_mask_generator_push_constant = {0};

  int32_t chunk_index = 0;
  int32_t chunk_count = STATIC_VDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    static_vdb_mask_generator_push_constant.chunk_position = static_vdb_chunk_index_to_position(chunk_index);
    static_vdb_mask_generator_push_constant.chunk_index = chunk_index;

    vkCmdPushConstants(g_window.command_buffer, g_renderer.static_vdb_mask_generator_pipeline.pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(static_vdb_mask_generator_push_constant), &static_vdb_mask_generator_push_constant);
    vkCmdDispatch(g_window.command_buffer, STATIC_VDB_CHUNK_SIZE, STATIC_VDB_CHUNK_SIZE, 6);

    chunk_index++;
  }

  VkBufferMemoryBarrier buffer_memory_barrier = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
    .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
    .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .buffer = g_static_vdb.chunk_mask_buffer.buffer_handle,
    .offset = 0,
    .size = VK_WHOLE_SIZE,
  };

  vkCmdPipelineBarrier(
    g_window.command_buffer,
    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
    0,
    0,
    0,
    1,
    &buffer_memory_barrier,
    0,
    0);
}
static void static_vdb_generate_mesh(void) {
  static_vdb_chunk_info_t *chunk_info = (static_vdb_chunk_info_t *)g_static_vdb.chunk_info_buffer.device_data;

  vkCmdBindPipeline(g_window.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, g_renderer.static_vdb_mesh_generator_pipeline.pipeline_handle);

  static_vdb_mesh_generator_push_constant_t static_vdb_mesh_generator_push_constant = {0};

  int32_t chunk_index = 0;
  int32_t chunk_count = STATIC_VDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    // TODO: find a clean way to reset these counters..
    chunk_info[chunk_index].vertex_count = 0;
    chunk_info[chunk_index].index_count = 0;

    static_vdb_mesh_generator_push_constant.chunk_position = static_vdb_chunk_index_to_position(chunk_index);
    static_vdb_mesh_generator_push_constant.chunk_index = chunk_index;

    vkCmdBindDescriptorSets(g_window.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, g_renderer.static_vdb_mesh_generator_pipeline.pipeline_layout, 0, 1, &g_renderer.static_vdb_mesh_generator_pipeline.descriptor_set[chunk_index], 0, 0);
    vkCmdPushConstants(g_window.command_buffer, g_renderer.static_vdb_mesh_generator_pipeline.pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(static_vdb_mesh_generator_push_constant), &static_vdb_mesh_generator_push_constant);
    vkCmdDispatch(g_window.command_buffer, STATIC_VDB_CHUNK_SIZE, 1, 6);

    VkBufferMemoryBarrier buffer_memory_barrier[] = {
      {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .buffer = g_static_vdb.chunk_vertex_buffer[chunk_index].buffer_handle,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
      },
      {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_INDEX_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .buffer = g_static_vdb.chunk_index_buffer[chunk_index].buffer_handle,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
      },
    };

    vkCmdPipelineBarrier(
      g_window.command_buffer,
      VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
      VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
      0,
      0,
      0,
      ARRAY_COUNT(buffer_memory_barrier),
      buffer_memory_barrier,
      0,
      0);

    chunk_index++;
  }
}

static void static_vdb_destroy_image(void) {
  int32_t chunk_index = 0;
  int32_t chunk_count = STATIC_VDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    image_destroy(&g_static_vdb.chunk_voxel_image[chunk_index]);

    chunk_index++;
  }

  HEAP_FREE(g_static_vdb.chunk_voxel_image);

  HEAP_FREE(g_static_vdb.chunk_voxel_descriptor_image_info);
}
static void static_vdb_destroy_buffer(void) {
  buffer_destroy(&g_static_vdb.chunk_info_buffer);
  buffer_destroy(&g_static_vdb.chunk_mask_buffer);

  int32_t chunk_index = 0;
  int32_t chunk_count = STATIC_VDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    buffer_destroy(&g_static_vdb.chunk_vertex_buffer[chunk_index]);
    buffer_destroy(&g_static_vdb.chunk_index_buffer[chunk_index]);

    chunk_index++;
  }

  HEAP_FREE(g_static_vdb.chunk_vertex_buffer);
  HEAP_FREE(g_static_vdb.chunk_index_buffer);

  HEAP_FREE(g_static_vdb.chunk_vertex_descriptor_buffer_info);
  HEAP_FREE(g_static_vdb.chunk_index_descriptor_buffer_info);
}
