#include <pch.h>

static void dvdb_create_chunk_voxel_image(void);

static void dvdb_create_chunk_info_buffer(void);
static void dvdb_create_aabb_buffer(void);
static void dvdb_create_instance_buffer(void);
static void dvdb_create_blas_buffer(void);
static void dvdb_create_tlas_buffer(void);
static void dvdb_create_blas_scratch_buffer(void);
static void dvdb_create_tlas_scratch_buffer(void);

static void dvdb_create_blas(void);
static void dvdb_create_tlas(void);

static void dvdb_build_blas(void);
static void dvdb_build_tlas(void);

static void dvdb_update_renderer_descriptor_set(void);

static void dvdb_destroy_image(void);
static void dvdb_destroy_buffer(void);
static void dvdb_destroy_blas(void);
static void dvdb_destroy_tlas(void);

// TODO: remove me..
static VkAabbPositionsKHR s_aabb = {
  .minX = 0.0F,
  .minY = 0.0F,
  .minZ = 0.0F,
  .maxX = 32.0F,
  .maxY = 32.0F,
  .maxZ = 32.0F,
};

static VkDescriptorPoolSize const s_renderer_descriptor_pool_size[] = {
  {
    .type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
    .descriptorCount = SWAPCHAIN_IMAGE_COUNT, // TODO: make seperate executables for single/double and tripple buffering.. (only static way!)
  },
  {
    .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = SWAPCHAIN_IMAGE_COUNT, // TODO: make seperate executables for single/double and tripple buffering.. (only static way!)
  },
  {
    .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    .descriptorCount = SWAPCHAIN_IMAGE_COUNT, // TODO: make seperate executables for single/double and tripple buffering.. (only static way!)
  },
  {
    .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    .descriptorCount = SWAPCHAIN_IMAGE_COUNT, // TODO: make seperate executables for single/double and tripple buffering.. (only static way!)
  },
};

static VkDescriptorSetLayoutBinding const s_renderer_descriptor_set_layout_binding[] = {
  {
    .binding = 0,
    .descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR,
    .pImmutableSamplers = 0,
  },
  {
    .binding = 1,
    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR,
    .pImmutableSamplers = 0,
  },
  {
    .binding = 2,
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR,
    .pImmutableSamplers = 0,
  },
  {
    .binding = 3,
    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR,
    .pImmutableSamplers = 0,
  },
};

static VkAccelerationStructureGeometryKHR s_blas_geometry = {0};
static VkAccelerationStructureGeometryKHR s_tlas_geometry = {0};

static VkAccelerationStructureBuildGeometryInfoKHR s_blas_build_geometry_info = {0}; // TODO: make queries in a separate function..
static VkAccelerationStructureBuildGeometryInfoKHR s_tlas_build_geometry_info = {0};

static VkAccelerationStructureBuildSizesInfoKHR s_blas_build_sizes_info = {
  .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR,
};
static VkAccelerationStructureBuildSizesInfoKHR s_tlas_build_sizes_info = {
  .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR,
};

static VkAccelerationStructureKHR s_blas = {0};
static VkAccelerationStructureKHR s_tlas = {0};

static buffer_t s_chunk_info_buffer = {
  .size = sizeof(dvdb_chunk_info_t) * DVDB_CHUNK_COUNT,
  .buffer_usage_flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
};
static buffer_t s_aabb_buffer = {
  .host_data = &s_aabb,
  .size = sizeof(s_aabb),
  .buffer_usage_flags = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
  .memory_allocate_flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
};
static buffer_t s_instance_buffer = {
  .buffer_usage_flags = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
  .memory_allocate_flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
};
static buffer_t s_blas_buffer = {
  .buffer_usage_flags = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
  .memory_allocate_flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
};
static buffer_t s_tlas_buffer = {
  .buffer_usage_flags = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
  .memory_allocate_flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
};
static buffer_t s_blas_scratch_buffer = {
  .buffer_usage_flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
  .memory_allocate_flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
};
static buffer_t s_tlas_scratch_buffer = {
  .buffer_usage_flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
  .memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
  .memory_allocate_flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
};

static image_t *s_curr_chunk_voxel_image = 0;
static image_t *s_next_chunk_voxel_image = 0;

static VkDescriptorBufferInfo s_chunk_info_descriptor_buffer_info = {0};

static VkDescriptorImageInfo *s_curr_chunk_voxel_descriptor_image_info = 0;
static VkDescriptorImageInfo *s_next_chunk_voxel_descriptor_image_info = 0;

static VkWriteDescriptorSetAccelerationStructureKHR s_blas_write_descriptor_set = {0};
static VkWriteDescriptorSetAccelerationStructureKHR s_tlas_write_descriptor_set = {0};

static pipeline_t s_renderer_pipeline = {
  .pipeline_type = PIPELINE_TYPE_RAY,
  .ray_gen_shader = ROOT_DIR "/shader/dvdb/renderer.rgen.spv",
  .ray_miss_shader = ROOT_DIR "/shader/dvdb/renderer.rmiss.spv",
  .ray_closest_hit_shader = ROOT_DIR "/shader/dvdb/renderer.rchit.spv",
  .ray_intersect_shader = ROOT_DIR "/shader/dvdb/renderer.rint.spv",
  .descriptor_pool_size = s_renderer_descriptor_pool_size,
  .descriptor_pool_size_count = ARRAY_COUNT(s_renderer_descriptor_pool_size),
  .descriptor_set_layout_binding = s_renderer_descriptor_set_layout_binding,
  .descriptor_set_layout_binding_count = ARRAY_COUNT(s_renderer_descriptor_set_layout_binding),
  .ray_gen_group_count = 1,
  .ray_miss_group_count = 1,
  .ray_hit_group_count = 1,
  .callable_group_count = 0,
  .descriptor_set_count = SWAPCHAIN_IMAGE_COUNT, // TODO: make seperate executables for single/double and tripple buffering.. (only static way!)
};

dvdb_t g_dvdb = {0};

void dvdb_create(void) {
  g_dvdb.geometry_count = 1;
  g_dvdb.primitive_count = 1;
  g_dvdb.instance_count = 1;

  dvdb_create_chunk_voxel_image();

  dvdb_create_chunk_info_buffer();

  // TODO: Begin BLAS creation
  dvdb_create_aabb_buffer();
  dvdb_create_blas_buffer();

  dvdb_create_blas();
  dvdb_create_blas_scratch_buffer();

  dvdb_build_blas();

  // TODO: Begin TLAS creation
  dvdb_create_instance_buffer();
  dvdb_create_tlas_buffer();

  dvdb_create_tlas();
  dvdb_create_tlas_scratch_buffer();

  dvdb_build_tlas();

  pipeline_create(&s_renderer_pipeline);

  dvdb_update_renderer_descriptor_set();
}
void dvdb_draw(void) {
  vkCmdBindPipeline(g_renderer.command_buffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, s_renderer_pipeline.pipeline_handle);
  vkCmdBindDescriptorSets(g_renderer.command_buffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, s_renderer_pipeline.pipeline_layout, 0, 1, &s_renderer_pipeline.descriptor_set[g_renderer.image_index], 0, 0);

  VkStridedDeviceAddressRegionKHR *ray_gen_region = &s_renderer_pipeline.ray_gen_region;
  VkStridedDeviceAddressRegionKHR *ray_miss_region = &s_renderer_pipeline.ray_miss_region;
  VkStridedDeviceAddressRegionKHR *ray_hit_region = &s_renderer_pipeline.ray_hit_region;
  VkStridedDeviceAddressRegionKHR *callable_region = &s_renderer_pipeline.callable_region;

  vkCmdTraceRaysKHR_proc(g_renderer.command_buffer, ray_gen_region, ray_miss_region, ray_hit_region, callable_region, g_window.window_width, g_window.window_height, 1);
}
void dvdb_destroy(void) {
  pipeline_destroy(&s_renderer_pipeline);

  dvdb_destroy_image();
  dvdb_destroy_buffer();
  dvdb_destroy_blas();
  dvdb_destroy_tlas();
}

uint32_t dvdb_chunk_position_to_index(ivector3_t chunk_position) {
  return (chunk_position.x) +
         (chunk_position.y * DVDB_DIM_X) +
         (chunk_position.z * DVDB_DIM_X * DVDB_DIM_Y);
}
ivector3_t dvdb_chunk_index_to_position(uint32_t chunk_index) {
  return (ivector3_t){
    chunk_index % DVDB_DIM_X,
    (chunk_index / DVDB_DIM_X) % DVDB_DIM_Y,
    chunk_index / (DVDB_DIM_X * DVDB_DIM_Y),
  };
}

static void dvdb_create_chunk_voxel_image(void) {
  s_curr_chunk_voxel_image = (image_t *)HEAP_ALLOC(sizeof(image_t) * DVDB_CHUNK_COUNT, 1, 0);
  s_next_chunk_voxel_image = (image_t *)HEAP_ALLOC(sizeof(image_t) * DVDB_CHUNK_COUNT, 1, 0);

  s_curr_chunk_voxel_descriptor_image_info = (VkDescriptorImageInfo *)HEAP_ALLOC(sizeof(VkDescriptorImageInfo) * DVDB_CHUNK_COUNT, 1, 0);
  s_next_chunk_voxel_descriptor_image_info = (VkDescriptorImageInfo *)HEAP_ALLOC(sizeof(VkDescriptorImageInfo) * DVDB_CHUNK_COUNT, 1, 0);

  uint32_t chunk_index = 0;
  uint32_t chunk_count = DVDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    s_curr_chunk_voxel_image[chunk_index].width = DVDB_CHUNK_SIZE;
    s_curr_chunk_voxel_image[chunk_index].height = DVDB_CHUNK_SIZE;
    s_curr_chunk_voxel_image[chunk_index].depth = DVDB_CHUNK_SIZE;
    s_curr_chunk_voxel_image[chunk_index].channel = 1;
    s_curr_chunk_voxel_image[chunk_index].element_size = sizeof(uint32_t);
    s_curr_chunk_voxel_image[chunk_index].format = VK_FORMAT_R32_UINT;
    s_curr_chunk_voxel_image[chunk_index].filter = VK_FILTER_NEAREST;
    s_curr_chunk_voxel_image[chunk_index].image_usage_flags = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    s_curr_chunk_voxel_image[chunk_index].image_type = VK_IMAGE_TYPE_3D;
    s_curr_chunk_voxel_image[chunk_index].image_view_type = VK_IMAGE_VIEW_TYPE_3D;
    s_curr_chunk_voxel_image[chunk_index].image_aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT;
    s_curr_chunk_voxel_image[chunk_index].image_tiling = VK_IMAGE_TILING_OPTIMAL;

    s_next_chunk_voxel_image[chunk_index].width = DVDB_CHUNK_SIZE;
    s_next_chunk_voxel_image[chunk_index].height = DVDB_CHUNK_SIZE;
    s_next_chunk_voxel_image[chunk_index].depth = DVDB_CHUNK_SIZE;
    s_next_chunk_voxel_image[chunk_index].channel = 1;
    s_next_chunk_voxel_image[chunk_index].element_size = sizeof(uint32_t);
    s_next_chunk_voxel_image[chunk_index].format = VK_FORMAT_R32_UINT;
    s_next_chunk_voxel_image[chunk_index].filter = VK_FILTER_NEAREST;
    s_next_chunk_voxel_image[chunk_index].image_usage_flags = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    s_next_chunk_voxel_image[chunk_index].image_type = VK_IMAGE_TYPE_3D;
    s_next_chunk_voxel_image[chunk_index].image_view_type = VK_IMAGE_VIEW_TYPE_3D;
    s_next_chunk_voxel_image[chunk_index].image_aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT;
    s_next_chunk_voxel_image[chunk_index].image_tiling = VK_IMAGE_TILING_OPTIMAL;

    image_create(&s_curr_chunk_voxel_image[chunk_index]);
    image_create(&s_next_chunk_voxel_image[chunk_index]);

    s_curr_chunk_voxel_descriptor_image_info[chunk_index].sampler = s_curr_chunk_voxel_image[chunk_index].sampler;
    s_curr_chunk_voxel_descriptor_image_info[chunk_index].imageView = s_curr_chunk_voxel_image[chunk_index].image_view;
    s_curr_chunk_voxel_descriptor_image_info[chunk_index].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    s_next_chunk_voxel_descriptor_image_info[chunk_index].sampler = s_next_chunk_voxel_image[chunk_index].sampler;
    s_next_chunk_voxel_descriptor_image_info[chunk_index].imageView = s_next_chunk_voxel_image[chunk_index].image_view;
    s_next_chunk_voxel_descriptor_image_info[chunk_index].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    chunk_index++;
  }
}

static void dvdb_create_chunk_info_buffer(void) {
  buffer_create(&s_chunk_info_buffer);

  buffer_map(&s_chunk_info_buffer);

  g_dvdb.chunk_info = (dvdb_chunk_info_t *)s_chunk_info_buffer.device_data;

  s_chunk_info_descriptor_buffer_info.offset = 0;
  s_chunk_info_descriptor_buffer_info.buffer = s_chunk_info_buffer.buffer_handle;
  s_chunk_info_descriptor_buffer_info.range = VK_WHOLE_SIZE;
}
static void dvdb_create_aabb_buffer(void) {
  buffer_create(&s_aabb_buffer);
}
static void dvdb_create_instance_buffer(void) {
  VkAccelerationStructureDeviceAddressInfoKHR blas_device_address_info = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR,
    .accelerationStructure = s_blas,
  };

  VkDeviceAddress blas_device_address = vkGetAccelerationStructureDeviceAddressKHR_proc(g_window.device, &blas_device_address_info);

  VkAccelerationStructureInstanceKHR acceleration_structure_instance = {
    .transform = {
      {
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
      },
    },
    .instanceCustomIndex = 0,
    .mask = 0xFF,
    .instanceShaderBindingTableRecordOffset = 0,
    .flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR,
    .accelerationStructureReference = blas_device_address,
  };

  s_instance_buffer.host_data = &acceleration_structure_instance;
  s_instance_buffer.size = sizeof(acceleration_structure_instance);

  buffer_create(&s_instance_buffer);
}
static void dvdb_create_blas_buffer(void) {
  VkBufferDeviceAddressInfo aabb_buffer_device_address_info = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
    .buffer = s_aabb_buffer.buffer_handle,
  };

  VkDeviceAddress aabb_device_address = vkGetBufferDeviceAddress(g_window.device, &aabb_buffer_device_address_info);

  s_blas_geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
  s_blas_geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
  s_blas_geometry.geometryType = VK_GEOMETRY_TYPE_AABBS_KHR;
  s_blas_geometry.geometry.aabbs.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR;
  s_blas_geometry.geometry.aabbs.stride = sizeof(s_aabb);
  s_blas_geometry.geometry.aabbs.data.deviceAddress = aabb_device_address;

  s_blas_build_geometry_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR,
  s_blas_build_geometry_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
  s_blas_build_geometry_info.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR,
  s_blas_build_geometry_info.geometryCount = g_dvdb.geometry_count,
  s_blas_build_geometry_info.pGeometries = &s_blas_geometry,

  vkGetAccelerationStructureBuildSizesKHR_proc(g_window.device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &s_blas_build_geometry_info, &g_dvdb.primitive_count, &s_blas_build_sizes_info);

  s_blas_buffer.size = s_blas_build_sizes_info.accelerationStructureSize;

  buffer_create(&s_blas_buffer);
}
static void dvdb_create_tlas_buffer(void) {
  VkBufferDeviceAddressInfo instance_buffer_device_address_info = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
    .buffer = s_instance_buffer.buffer_handle,
  };

  VkDeviceAddress instance_device_address = vkGetBufferDeviceAddress(g_window.device, &instance_buffer_device_address_info);

  s_tlas_geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
  s_tlas_geometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
  s_tlas_geometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
  s_tlas_geometry.geometry.instances.data.deviceAddress = instance_device_address;

  s_tlas_build_geometry_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
  s_tlas_build_geometry_info.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
  s_tlas_build_geometry_info.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
  s_tlas_build_geometry_info.geometryCount = 1; // TODO: this should be one right..?
  s_tlas_build_geometry_info.pGeometries = &s_tlas_geometry;

  vkGetAccelerationStructureBuildSizesKHR_proc(g_window.device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &s_tlas_build_geometry_info, &g_dvdb.instance_count, &s_tlas_build_sizes_info);

  s_tlas_buffer.size = s_tlas_build_sizes_info.accelerationStructureSize;

  buffer_create(&s_tlas_buffer);
}
static void dvdb_create_blas_scratch_buffer(void) {
  s_blas_scratch_buffer.size = s_blas_build_sizes_info.buildScratchSize;

  buffer_create(&s_blas_scratch_buffer);
}
static void dvdb_create_tlas_scratch_buffer(void) {
  s_tlas_scratch_buffer.size = s_tlas_build_sizes_info.buildScratchSize;

  buffer_create(&s_tlas_scratch_buffer);
}

static void dvdb_create_blas(void) {
  VkAccelerationStructureCreateInfoKHR acceleration_structure_create_info = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR,
    .buffer = s_blas_buffer.buffer_handle,
    .size = s_blas_build_sizes_info.accelerationStructureSize,
    .type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
  };

  VK_CHECK(vkCreateAccelerationStructureKHR_proc(g_window.device, &acceleration_structure_create_info, 0, &s_blas));

  s_blas_write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
  s_blas_write_descriptor_set.pNext = 0;
  s_blas_write_descriptor_set.accelerationStructureCount = 1;    // TODO
  s_blas_write_descriptor_set.pAccelerationStructures = &s_blas; // TODO
}
static void dvdb_create_tlas(void) {
  VkAccelerationStructureCreateInfoKHR acceleration_structure_create_info = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR,
    .buffer = s_tlas_buffer.buffer_handle,
    .size = s_tlas_build_sizes_info.accelerationStructureSize,
    .type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR,
  };

  VK_CHECK(vkCreateAccelerationStructureKHR_proc(g_window.device, &acceleration_structure_create_info, 0, &s_tlas));

  s_tlas_write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
  s_tlas_write_descriptor_set.pNext = 0;
  s_tlas_write_descriptor_set.accelerationStructureCount = 1;    // TODO
  s_tlas_write_descriptor_set.pAccelerationStructures = &s_tlas; // TODO
}

static void dvdb_build_blas(void) {
  VkBufferDeviceAddressInfo blas_buffer_device_address_info = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
    .buffer = s_blas_scratch_buffer.buffer_handle,
  };

  VkDeviceAddress blas_scratch_device_address = vkGetBufferDeviceAddress(g_window.device, &blas_buffer_device_address_info);

  // TODO: set scratch data device address and BLAS..
  s_blas_build_geometry_info.dstAccelerationStructure = s_blas;
  s_blas_build_geometry_info.scratchData.deviceAddress = blas_scratch_device_address;

  VkAccelerationStructureBuildRangeInfoKHR dflt_acceleration_structure_build_range_info = {
    .primitiveCount = g_dvdb.primitive_count,
  };

  VkAccelerationStructureBuildRangeInfoKHR const *acceleration_structure_build_range_info[] = {
    &dflt_acceleration_structure_build_range_info,
  };

  VkCommandBuffer command_buffer = vkutil_primary_command_buffer_record_immediate();

  vkCmdBuildAccelerationStructuresKHR_proc(command_buffer, 1, &s_blas_build_geometry_info, acceleration_structure_build_range_info);

  vkutil_primary_command_buffer_submit_immediate(command_buffer);
}
static void dvdb_build_tlas(void) {
  VkBufferDeviceAddressInfo tlas_buffer_device_address_info = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
    .buffer = s_tlas_scratch_buffer.buffer_handle,
  };

  VkDeviceAddress tlas_scratch_device_address = vkGetBufferDeviceAddress(g_window.device, &tlas_buffer_device_address_info);

  // TODO: set scratch data device address and TLAS..
  s_tlas_build_geometry_info.dstAccelerationStructure = s_tlas;
  s_tlas_build_geometry_info.scratchData.deviceAddress = tlas_scratch_device_address;

  VkAccelerationStructureBuildRangeInfoKHR dflt_acceleration_structure_build_range_info = {
    .primitiveCount = g_dvdb.instance_count, // TODO
  };

  VkAccelerationStructureBuildRangeInfoKHR const *acceleration_structure_build_range_info[] = {
    &dflt_acceleration_structure_build_range_info,
  };

  VkCommandBuffer command_buffer = vkutil_primary_command_buffer_record_immediate();

  vkCmdBuildAccelerationStructuresKHR_proc(command_buffer, 1, &s_tlas_build_geometry_info, acceleration_structure_build_range_info);

  vkutil_primary_command_buffer_submit_immediate(command_buffer);
}

static void dvdb_update_renderer_descriptor_set(void) {
  uint32_t image_index = 0;
  uint32_t image_count = g_swapchain.image_count;

  while (image_index < image_count) {

    VkWriteDescriptorSet write_descriptor_set[] = {
      {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = &s_tlas_write_descriptor_set,
        .dstSet = s_renderer_pipeline.descriptor_set[image_index],
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
        .descriptorCount = 1,
        .pImageInfo = 0,
        .pBufferInfo = 0,
        .pTexelBufferView = 0,
      },
      {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = 0,
        .dstSet = s_renderer_pipeline.descriptor_set[image_index],
        .dstBinding = 1,
        .dstArrayElement = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .pImageInfo = 0,
        .pBufferInfo = &g_renderer.camera_info_descriptor_buffer_info,
        .pTexelBufferView = 0,
      },
      {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = 0,
        .dstSet = s_renderer_pipeline.descriptor_set[image_index],
        .dstBinding = 2,
        .dstArrayElement = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        .descriptorCount = 1,
        .pImageInfo = &g_framebuffer.color_descriptor_image_info[image_index],
        .pBufferInfo = 0,
        .pTexelBufferView = 0,
      },
      {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = 0,
        .dstSet = s_renderer_pipeline.descriptor_set[image_index],
        .dstBinding = 3,
        .dstArrayElement = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .pImageInfo = &g_framebuffer.depth_descriptor_image_info[image_index],
        .pBufferInfo = 0,
        .pTexelBufferView = 0,
      },
    };

    vkUpdateDescriptorSets(g_window.device, ARRAY_COUNT(write_descriptor_set), write_descriptor_set, 0, 0);

    image_index++;
  }
}

static void dvdb_destroy_image(void) {
  uint32_t chunk_index = 0;
  uint32_t chunk_count = DVDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    image_destroy(&s_curr_chunk_voxel_image[chunk_index]);
    image_destroy(&s_next_chunk_voxel_image[chunk_index]);

    chunk_index++;
  }

  HEAP_FREE(s_curr_chunk_voxel_image);
  HEAP_FREE(s_next_chunk_voxel_image);

  HEAP_FREE(s_curr_chunk_voxel_descriptor_image_info);
  HEAP_FREE(s_next_chunk_voxel_descriptor_image_info);
}
static void dvdb_destroy_buffer(void) {
  buffer_destroy(&s_chunk_info_buffer);
  buffer_destroy(&s_aabb_buffer);
  buffer_destroy(&s_instance_buffer);
  buffer_destroy(&s_blas_buffer);
  buffer_destroy(&s_tlas_buffer);
  buffer_destroy(&s_blas_scratch_buffer);
  buffer_destroy(&s_tlas_scratch_buffer);
}
static void dvdb_destroy_blas(void) {
  vkDestroyAccelerationStructureKHR_proc(g_window.device, s_blas, 0);
}
static void dvdb_destroy_tlas(void) {
  vkDestroyAccelerationStructureKHR_proc(g_window.device, s_tlas, 0);
}
