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

static void dynamic_vdb_create_chunk_voxel_image(void);

static void dynamic_vdb_create_chunk_info_buffer(void);
static void dynamic_vdb_create_aabb_buffer(void);
static void dynamic_vdb_create_blas_buffer(void);
static void dynamic_vdb_create_scratch_buffer(void);

static void dynamic_vdb_create_blas(void);

static void dynamic_vdb_build_blas(void);

static void dynamic_vdb_destroy_image(void);
static void dynamic_vdb_destroy_buffer(void);

// TODO: remove me..
static VkAabbPositionsKHR s_aabb = {
  .minX = -1.0f,
  .minY = -1.0f,
  .minZ = -1.0f,
  .maxX = 1.0f,
  .maxY = 1.0f,
  .maxZ = 1.0f,
};

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

dynamic_vdb_t g_dynamic_vdb = {
  .chunk_info_buffer = {
    .size = sizeof(dynamic_vdb_chunk_info_t) * DYNAMIC_VDB_CHUNK_COUNT,
    .buffer_usage_flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
    .memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
  },
  .aabb_buffer = {
    .host_data = &s_aabb,
    .size = sizeof(s_aabb),
    .buffer_usage_flags = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
    .memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    .memory_allocate_flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
  },
  .blas_buffer = {
    .buffer_usage_flags = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
    .memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    .memory_allocate_flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
  },
  .scratch_buffer = {
    .buffer_usage_flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
    .memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    .memory_allocate_flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
  },
  .geometry_count = 1,
  .primitive_count = 1,
  .blas_build_sizes_info = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR,
  },
};

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
  int32_t chunk_index = 0;
  int32_t chunk_count = STATIC_VDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    ivector3_t chunk_position = static_vdb_chunk_index_to_position(chunk_index);

    vector4_t chunk_color = {0};

    if (g_static_vdb.chunk_info[chunk_index].is_dirty) {
      chunk_color = vector4_xyzw(1.0F, 0.0F, 0.0F, 1.0F);
    } else {
      chunk_color = vector4_xyzw(1.0F, 1.0F, 1.0F, 1.0F);
    }

    renderer_draw_debug_box(
      (vector3_t){(float)chunk_position.x * STATIC_VDB_CHUNK_SIZE, (float)chunk_position.y * STATIC_VDB_CHUNK_SIZE, (float)chunk_position.z * STATIC_VDB_CHUNK_SIZE},
      (vector3_t){(float)STATIC_VDB_CHUNK_SIZE, (float)STATIC_VDB_CHUNK_SIZE, (float)STATIC_VDB_CHUNK_SIZE},
      chunk_color);

    chunk_index++;
  }
}
void static_vdb_draw(void) {
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
    vkCmdDrawIndexed(g_window.command_buffer, g_static_vdb.chunk_info[chunk_index].index_count, 1, 0, 0, 0);

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
  dynamic_vdb_create_chunk_voxel_image();

  dynamic_vdb_create_chunk_info_buffer();
  dynamic_vdb_create_aabb_buffer();
  dynamic_vdb_create_blas_buffer();

  dynamic_vdb_create_blas();

  dynamic_vdb_create_scratch_buffer();

  dynamic_vdb_build_blas();
}
void dynamic_vdb_draw(void) {
  vkCmdBindPipeline(g_window.command_buffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, g_renderer.dynamic_vdb_renderer_pipeline.pipeline_handle);
  vkCmdBindDescriptorSets(g_window.command_buffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, g_renderer.dynamic_vdb_renderer_pipeline.pipeline_layout, 0, 1, &g_renderer.dynamic_vdb_renderer_pipeline.descriptor_set[0], 0, 0);

  VkStridedDeviceAddressRegionKHR *ray_gen_region = &g_renderer.dynamic_vdb_renderer_pipeline.ray_gen_region;
  VkStridedDeviceAddressRegionKHR *ray_miss_region = &g_renderer.dynamic_vdb_renderer_pipeline.ray_miss_region;
  VkStridedDeviceAddressRegionKHR *ray_hit_region = &g_renderer.dynamic_vdb_renderer_pipeline.ray_hit_region;
  VkStridedDeviceAddressRegionKHR *callable_region = &g_renderer.dynamic_vdb_renderer_pipeline.callable_region;

  vkCmdTraceRaysKHR_proc(g_window.command_buffer, ray_gen_region, ray_miss_region, ray_hit_region, callable_region, g_window.window_width, g_window.window_height, 1);
}
void dynamic_vdb_debug(void) {
  int32_t chunk_index = 0;
  int32_t chunk_count = DYNAMIC_VDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    ivector3_t chunk_position = dynamic_vdb_chunk_index_to_position(chunk_index);

    vector4_t chunk_color = {0};

    if (g_dynamic_vdb.chunk_info[chunk_index].is_dirty) {
      chunk_color = vector4_xyzw(1.0F, 0.0F, 0.0F, 1.0F);
    } else {
      chunk_color = vector4_xyzw(1.0F, 1.0F, 1.0F, 1.0F);
    }

    renderer_draw_debug_box(
      (vector3_t){(float)chunk_position.x * DYNAMIC_VDB_CHUNK_SIZE, (float)chunk_position.y * DYNAMIC_VDB_CHUNK_SIZE, (float)chunk_position.z * DYNAMIC_VDB_CHUNK_SIZE},
      (vector3_t){(float)DYNAMIC_VDB_CHUNK_SIZE, (float)DYNAMIC_VDB_CHUNK_SIZE, (float)DYNAMIC_VDB_CHUNK_SIZE},
      chunk_color);

    chunk_index++;
  }
}
void dynamic_vdb_destroy(void) {
  dynamic_vdb_destroy_image();
  dynamic_vdb_destroy_buffer();
}

int32_t dynamic_vdb_chunk_position_to_index(ivector3_t chunk_position) {
  return (chunk_position.x) +
         (chunk_position.y * DYNAMIC_VDB_DIM_X) +
         (chunk_position.z * DYNAMIC_VDB_DIM_X * DYNAMIC_VDB_DIM_Y);
}
ivector3_t dynamic_vdb_chunk_index_to_position(int32_t chunk_index) {
  return (ivector3_t){
    chunk_index % DYNAMIC_VDB_DIM_X,
    (chunk_index / DYNAMIC_VDB_DIM_X) % DYNAMIC_VDB_DIM_Y,
    chunk_index / (DYNAMIC_VDB_DIM_X * DYNAMIC_VDB_DIM_Y),
  };
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

  g_static_vdb.chunk_info = (static_vdb_chunk_info_t *)g_static_vdb.chunk_info_buffer.device_data;

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
  vkCmdBindPipeline(g_window.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, g_renderer.static_vdb_mesh_generator_pipeline.pipeline_handle);

  static_vdb_mesh_generator_push_constant_t static_vdb_mesh_generator_push_constant = {0};

  int32_t chunk_index = 0;
  int32_t chunk_count = STATIC_VDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    // TODO: find a clean way to reset these counters..
    g_static_vdb.chunk_info[chunk_index].vertex_count = 0;
    g_static_vdb.chunk_info[chunk_index].index_count = 0;

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

static void dynamic_vdb_create_chunk_voxel_image(void) {
  g_dynamic_vdb.curr_chunk_voxel_image = (image_t *)HEAP_ALLOC(sizeof(image_t) * DYNAMIC_VDB_CHUNK_COUNT, 1, 0);
  g_dynamic_vdb.next_chunk_voxel_image = (image_t *)HEAP_ALLOC(sizeof(image_t) * DYNAMIC_VDB_CHUNK_COUNT, 1, 0);

  g_dynamic_vdb.curr_chunk_voxel_descriptor_image_info = (VkDescriptorImageInfo *)HEAP_ALLOC(sizeof(VkDescriptorImageInfo) * DYNAMIC_VDB_CHUNK_COUNT, 1, 0);
  g_dynamic_vdb.next_chunk_voxel_descriptor_image_info = (VkDescriptorImageInfo *)HEAP_ALLOC(sizeof(VkDescriptorImageInfo) * DYNAMIC_VDB_CHUNK_COUNT, 1, 0);

  int32_t chunk_index = 0;
  int32_t chunk_count = DYNAMIC_VDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    g_dynamic_vdb.curr_chunk_voxel_image[chunk_index].width = DYNAMIC_VDB_CHUNK_SIZE,
    g_dynamic_vdb.curr_chunk_voxel_image[chunk_index].height = DYNAMIC_VDB_CHUNK_SIZE,
    g_dynamic_vdb.curr_chunk_voxel_image[chunk_index].depth = DYNAMIC_VDB_CHUNK_SIZE,
    g_dynamic_vdb.curr_chunk_voxel_image[chunk_index].channel = 1,
    g_dynamic_vdb.curr_chunk_voxel_image[chunk_index].element_size = sizeof(uint32_t),
    g_dynamic_vdb.curr_chunk_voxel_image[chunk_index].format = VK_FORMAT_R32_UINT,
    g_dynamic_vdb.curr_chunk_voxel_image[chunk_index].filter = VK_FILTER_NEAREST,
    g_dynamic_vdb.curr_chunk_voxel_image[chunk_index].image_usage_flags = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
    g_dynamic_vdb.curr_chunk_voxel_image[chunk_index].image_type = VK_IMAGE_TYPE_3D,
    g_dynamic_vdb.curr_chunk_voxel_image[chunk_index].image_view_type = VK_IMAGE_VIEW_TYPE_3D,
    g_dynamic_vdb.curr_chunk_voxel_image[chunk_index].image_aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT,
    g_dynamic_vdb.curr_chunk_voxel_image[chunk_index].image_tiling = VK_IMAGE_TILING_OPTIMAL,

    g_dynamic_vdb.next_chunk_voxel_image[chunk_index].width = DYNAMIC_VDB_CHUNK_SIZE,
    g_dynamic_vdb.next_chunk_voxel_image[chunk_index].height = DYNAMIC_VDB_CHUNK_SIZE,
    g_dynamic_vdb.next_chunk_voxel_image[chunk_index].depth = DYNAMIC_VDB_CHUNK_SIZE,
    g_dynamic_vdb.next_chunk_voxel_image[chunk_index].channel = 1,
    g_dynamic_vdb.next_chunk_voxel_image[chunk_index].element_size = sizeof(uint32_t),
    g_dynamic_vdb.next_chunk_voxel_image[chunk_index].format = VK_FORMAT_R32_UINT,
    g_dynamic_vdb.next_chunk_voxel_image[chunk_index].filter = VK_FILTER_NEAREST,
    g_dynamic_vdb.next_chunk_voxel_image[chunk_index].image_usage_flags = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
    g_dynamic_vdb.next_chunk_voxel_image[chunk_index].image_type = VK_IMAGE_TYPE_3D,
    g_dynamic_vdb.next_chunk_voxel_image[chunk_index].image_view_type = VK_IMAGE_VIEW_TYPE_3D,
    g_dynamic_vdb.next_chunk_voxel_image[chunk_index].image_aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT,
    g_dynamic_vdb.next_chunk_voxel_image[chunk_index].image_tiling = VK_IMAGE_TILING_OPTIMAL,

    image_create(&g_dynamic_vdb.curr_chunk_voxel_image[chunk_index]);
    image_create(&g_dynamic_vdb.next_chunk_voxel_image[chunk_index]);

    g_dynamic_vdb.curr_chunk_voxel_descriptor_image_info[chunk_index].sampler = g_dynamic_vdb.curr_chunk_voxel_image[chunk_index].sampler;
    g_dynamic_vdb.curr_chunk_voxel_descriptor_image_info[chunk_index].imageView = g_dynamic_vdb.curr_chunk_voxel_image[chunk_index].image_view;
    g_dynamic_vdb.curr_chunk_voxel_descriptor_image_info[chunk_index].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    g_dynamic_vdb.next_chunk_voxel_descriptor_image_info[chunk_index].sampler = g_dynamic_vdb.next_chunk_voxel_image[chunk_index].sampler;
    g_dynamic_vdb.next_chunk_voxel_descriptor_image_info[chunk_index].imageView = g_dynamic_vdb.next_chunk_voxel_image[chunk_index].image_view;
    g_dynamic_vdb.next_chunk_voxel_descriptor_image_info[chunk_index].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    chunk_index++;
  }
}

static void dynamic_vdb_create_chunk_info_buffer(void) {
  buffer_create(&g_dynamic_vdb.chunk_info_buffer);

  buffer_map(&g_dynamic_vdb.chunk_info_buffer);

  g_dynamic_vdb.chunk_info = (dynamic_vdb_chunk_info_t *)g_dynamic_vdb.chunk_info_buffer.device_data;

  g_dynamic_vdb.chunk_info_descriptor_buffer_info.offset = 0;
  g_dynamic_vdb.chunk_info_descriptor_buffer_info.buffer = g_dynamic_vdb.chunk_info_buffer.buffer_handle;
  g_dynamic_vdb.chunk_info_descriptor_buffer_info.range = VK_WHOLE_SIZE;
}
static void dynamic_vdb_create_aabb_buffer(void) {
  buffer_create(&g_dynamic_vdb.aabb_buffer);

  buffer_map(&g_dynamic_vdb.aabb_buffer);
}
static void dynamic_vdb_create_blas_buffer(void) {
  VkBufferDeviceAddressInfo buffer_device_address_info = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
    .buffer = g_dynamic_vdb.aabb_buffer.buffer_handle,
  };

  VkDeviceAddress aabb_device_address = vkGetBufferDeviceAddress(g_window.device, &buffer_device_address_info);

  g_dynamic_vdb.blas_geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
  g_dynamic_vdb.blas_geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
  g_dynamic_vdb.blas_geometry.geometryType = VK_GEOMETRY_TYPE_AABBS_KHR;
  g_dynamic_vdb.blas_geometry.geometry.aabbs.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR;
  g_dynamic_vdb.blas_geometry.geometry.aabbs.stride = sizeof(s_aabb);
  g_dynamic_vdb.blas_geometry.geometry.aabbs.data.deviceAddress = aabb_device_address;

  g_dynamic_vdb.blas_build_geometry_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR,
  g_dynamic_vdb.blas_build_geometry_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
  g_dynamic_vdb.blas_build_geometry_info.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR,
  g_dynamic_vdb.blas_build_geometry_info.geometryCount = g_dynamic_vdb.geometry_count,
  g_dynamic_vdb.blas_build_geometry_info.pGeometries = &g_dynamic_vdb.blas_geometry,

  vkGetAccelerationStructureBuildSizesKHR_proc(g_window.device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &g_dynamic_vdb.blas_build_geometry_info, &g_dynamic_vdb.primitive_count, &g_dynamic_vdb.blas_build_sizes_info);

  g_dynamic_vdb.blas_buffer.size = g_dynamic_vdb.blas_build_sizes_info.accelerationStructureSize;

  buffer_create(&g_dynamic_vdb.blas_buffer);
}
static void dynamic_vdb_create_scratch_buffer(void) {
  g_dynamic_vdb.scratch_buffer.size = g_dynamic_vdb.blas_build_sizes_info.buildScratchSize;

  buffer_create(&g_dynamic_vdb.scratch_buffer);
}

static void dynamic_vdb_create_blas(void) {
  VkAccelerationStructureCreateInfoKHR acceleration_structure_create_info = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR,
    .buffer = g_dynamic_vdb.blas_buffer.buffer_handle,
    .size = g_dynamic_vdb.blas_build_sizes_info.accelerationStructureSize,
    .type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
  };

  VK_CHECK(vkCreateAccelerationStructureKHR_proc(g_window.device, &acceleration_structure_create_info, 0, &g_dynamic_vdb.blas));
}

static void dynamic_vdb_build_blas(void) {
  VkBufferDeviceAddressInfo buffer_device_address_info = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
    .buffer = g_dynamic_vdb.scratch_buffer.buffer_handle,
  };

  VkDeviceAddress scratch_device_address = vkGetBufferDeviceAddress(g_window.device, &buffer_device_address_info);

  // TODO: set scratch data device address and BLAS..
  g_dynamic_vdb.blas_build_geometry_info.scratchData.deviceAddress = scratch_device_address;
  g_dynamic_vdb.blas_build_geometry_info.dstAccelerationStructure = g_dynamic_vdb.blas;

  VkAccelerationStructureBuildRangeInfoKHR dflt_acceleration_structure_build_range_info = {
    .primitiveCount = g_dynamic_vdb.primitive_count,
  };

  VkAccelerationStructureBuildRangeInfoKHR const *acceleration_structure_build_range_info[] = {
    &dflt_acceleration_structure_build_range_info,
  };

  vkCmdBuildAccelerationStructuresKHR_proc(g_window.command_buffer, 1, &g_dynamic_vdb.blas_build_geometry_info, acceleration_structure_build_range_info);
}

static void dynamic_vdb_destroy_image(void) {
  int32_t chunk_index = 0;
  int32_t chunk_count = DYNAMIC_VDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    image_destroy(&g_dynamic_vdb.curr_chunk_voxel_image[chunk_index]);
    image_destroy(&g_dynamic_vdb.next_chunk_voxel_image[chunk_index]);

    chunk_index++;
  }

  HEAP_FREE(g_dynamic_vdb.curr_chunk_voxel_image);
  HEAP_FREE(g_dynamic_vdb.next_chunk_voxel_image);

  HEAP_FREE(g_dynamic_vdb.curr_chunk_voxel_descriptor_image_info);
  HEAP_FREE(g_dynamic_vdb.next_chunk_voxel_descriptor_image_info);
}
static void dynamic_vdb_destroy_buffer(void) {
  buffer_destroy(&g_dynamic_vdb.chunk_info_buffer);
  buffer_destroy(&g_dynamic_vdb.aabb_buffer);
  buffer_destroy(&g_dynamic_vdb.blas_buffer);
  buffer_destroy(&g_dynamic_vdb.scratch_buffer);
}
