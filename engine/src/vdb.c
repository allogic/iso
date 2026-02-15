#include <pch.h>

static void vdb_create_terrain_layer_buffer(void);
static void vdb_create_cluster_info_buffer(void);
static void vdb_create_chunk_info_buffer(void);
static void vdb_create_chunk_mask_buffer(void);
static void vdb_create_chunk_index_buffer(void);
static void vdb_create_chunk_image(void);
static void vdb_create_chunk_sampler(void);

vdb_t g_vdb = {0};

void vdb_create(void) {
  vdb_create_terrain_layer_buffer();
  vdb_create_cluster_info_buffer();
  vdb_create_chunk_info_buffer();
  vdb_create_chunk_mask_buffer();
  vdb_create_chunk_index_buffer();
  vdb_create_chunk_image();
  vdb_create_chunk_sampler();
}
void vdb_destroy(void) {
  buffer_destroy(&g_vdb.terrain_layer_buffer);
  buffer_destroy(&g_vdb.cluster_info_buffer);
  buffer_destroy(&g_vdb.chunk_info_buffer);
  buffer_destroy(&g_vdb.chunk_mask_buffer);
  buffer_destroy(&g_vdb.chunk_index_buffer);

  vkDestroyImageView(g_window.device, g_vdb.chunk_image_view, 0);
  vkFreeMemory(g_window.device, g_vdb.chunk_device_memory, 0);
  vkDestroyImage(g_window.device, g_vdb.chunk_image, 0);

  vkDestroySampler(g_window.device, g_vdb.chunk_sampler, 0);
}

int32_t vdb_chunk_position_to_index(ivector2_t chunk_position) {
  return (chunk_position.x) +
         (chunk_position.y * VDB_CLUSTER_DIM_X);
}
ivector2_t vdb_chunk_index_to_position(int32_t chunk_index) {
  return (ivector2_t){
    chunk_index % VDB_CLUSTER_DIM_X,
    (chunk_index / VDB_CLUSTER_DIM_X) % VDB_CLUSTER_DIM_Y,
  };
}

static void vdb_create_terrain_layer_buffer(void) {
  g_vdb.terrain_layer_buffer = buffer_create_uniform(0, sizeof(vdb_terrain_layer_t) * VDB_TERRAIN_LAYER_COUNT);
}
static void vdb_create_cluster_info_buffer(void) {
  vdb_cluster_info_t vdb_cluster_info = {
    .cluster_dim = {
      .x = VDB_CLUSTER_DIM_X,
      .y = VDB_CLUSTER_DIM_Y,
    },
  };

  g_vdb.cluster_info_buffer = buffer_create_uniform(&vdb_cluster_info, sizeof(vdb_cluster_info_t));
}
static void vdb_create_chunk_info_buffer(void) {
  vdb_chunk_info_t *chunk_info = (vdb_chunk_info_t *)HEAP_ALLOC(sizeof(vdb_chunk_info_t) * VDB_CHUNK_COUNT, 1, 0);

  int32_t chunk_index = 0;
  int32_t chunk_count = VDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    ivector2_t chunk_position = vdb_chunk_index_to_position(chunk_index);
    ivector2_t chunk_size = {VDB_CHUNK_SIZE, VDB_CHUNK_SIZE};

    chunk_info[chunk_index].chunk_position = chunk_position;
    chunk_info[chunk_index].chunk_size = chunk_size;

    chunk_index++;
  }

  g_vdb.chunk_info_buffer = buffer_create_storage(chunk_info, sizeof(vdb_chunk_info_t) * VDB_CHUNK_COUNT);

  HEAP_FREE(chunk_info);
}
static void vdb_create_chunk_mask_buffer(void) {
  g_vdb.chunk_mask_buffer = buffer_create_storage(0, sizeof(vdb_chunk_mask_t) * VDB_CHUNK_COUNT);
}
static void vdb_create_chunk_index_buffer(void) {
  g_vdb.chunk_index_buffer = buffer_create_storage_coherent(0, sizeof(uint32_t) * VDB_CHUNK_COUNT);
}
static void vdb_create_chunk_image(void) {
  VkImageCreateInfo image_create_info = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    .imageType = VK_IMAGE_TYPE_3D,
    .extent = {
      .width = VDB_CHUNK_SIZE,
      .height = VDB_CHUNK_SIZE,
      .depth = VDB_CHUNK_SIZE,
    },
    .mipLevels = 1,
    .arrayLayers = 1,
    .format = VK_FORMAT_R32_UINT,
    .tiling = VK_IMAGE_TILING_OPTIMAL,
    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    .usage = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
  };

  VK_CHECK(vkCreateImage(g_window.device, &image_create_info, 0, &g_vdb.chunk_image));

  VkMemoryRequirements memory_requirements = {0};

  vkGetImageMemoryRequirements(g_window.device, g_vdb.chunk_image, &memory_requirements);

  uint32_t memory_type_index = vkutils_find_memory_type_index(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  VkMemoryAllocateInfo memory_allocate_info = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
    .allocationSize = memory_requirements.size,
    .memoryTypeIndex = memory_type_index,
  };

  VK_CHECK(vkAllocateMemory(g_window.device, &memory_allocate_info, 0, &g_vdb.chunk_device_memory));
  VK_CHECK(vkBindImageMemory(g_window.device, g_vdb.chunk_image, g_vdb.chunk_device_memory, 0));

  VkImageViewCreateInfo image_view_create_info = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    .image = g_vdb.chunk_image,
    .viewType = VK_IMAGE_VIEW_TYPE_3D,
    .format = VK_FORMAT_R32_UINT,
    .subresourceRange = {
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .baseMipLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1,
    },
  };

  VK_CHECK(vkCreateImageView(g_window.device, &image_view_create_info, 0, &g_vdb.chunk_image_view));

  VkCommandBufferBeginInfo command_buffer_begin_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
  };

  VK_CHECK(vkBeginCommandBuffer(g_window.command_buffer, &command_buffer_begin_info));

  VkImageMemoryBarrier image_memory_barrier = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    .newLayout = VK_IMAGE_LAYOUT_GENERAL,
    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .image = g_vdb.chunk_image,
    .subresourceRange = {
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .baseMipLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1,
    },
    .srcAccessMask = VK_ACCESS_NONE,
    .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
  };

  vkCmdPipelineBarrier(
    g_window.command_buffer,
    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
    0,
    0,
    0,
    0,
    0,
    1,
    &image_memory_barrier);

  VK_CHECK(vkEndCommandBuffer(g_window.command_buffer));

  VkSubmitInfo submit_info = {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .commandBufferCount = 1,
    .pCommandBuffers = &g_window.command_buffer,
  };

  VK_CHECK(vkQueueSubmit(g_window.primary_queue, 1, &submit_info, 0));
  VK_CHECK(vkQueueWaitIdle(g_window.primary_queue));
}
static void vdb_create_chunk_sampler(void) {
  VkSamplerCreateInfo sampler_create_info = {
    .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
    .magFilter = VK_FILTER_NEAREST,
    .minFilter = VK_FILTER_NEAREST,
    .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    .anisotropyEnable = 0,
    .maxAnisotropy = 0.0F,
    .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
    .unnormalizedCoordinates = 0,
    .compareEnable = 0,
    .compareOp = VK_COMPARE_OP_ALWAYS,
    .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
    .mipLodBias = 0.0F,
    .minLod = 0.0F,
    .maxLod = VK_LOD_CLAMP_NONE,
  };

  VK_CHECK(vkCreateSampler(g_window.device, &sampler_create_info, 0, &g_vdb.chunk_sampler));
}
