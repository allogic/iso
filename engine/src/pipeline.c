#include <pch.h>

static void pipeline_create_descriptor_pool(pipeline_t *pipeline);
static void pipeline_create_descriptor_set_layout(pipeline_t *pipeline);
static void pipeline_create_descriptor_set(pipeline_t *pipeline);
static void pipeline_create_pipeline_layout(pipeline_t *pipeline);

static void pipeline_create_sbt_buffer(pipeline_t *pipeline);

static void pipeline_create_dflt(pipeline_t *pipeline);
static void pipeline_create_mesh(pipeline_t *pipeline);
static void pipeline_create_ray(pipeline_t *pipeline);
static void pipeline_create_comp(pipeline_t *pipeline);

static void pipeline_destroy_sbt_buffer(pipeline_t *pipeline);

void pipeline_create(pipeline_t *pipeline) {
  pipeline->descriptor_set_layout = (VkDescriptorSetLayout *)HEAP_ALLOC(sizeof(VkDescriptorSetLayout) * pipeline->descriptor_set_count, 0, 0);
  pipeline->descriptor_set = (VkDescriptorSet *)HEAP_ALLOC(sizeof(VkDescriptorSet) * pipeline->descriptor_set_count, 0, 0);

  pipeline_create_descriptor_pool(pipeline);
  pipeline_create_descriptor_set_layout(pipeline);
  pipeline_create_descriptor_set(pipeline);
  pipeline_create_pipeline_layout(pipeline);

  switch (pipeline->pipeline_type) {
    case PIPELINE_TYPE_DFLT: {

      pipeline_create_dflt(pipeline);

      break;
    }
    case PIPELINE_TYPE_MESH: {

      pipeline_create_mesh(pipeline);

      break;
    }
    case PIPELINE_TYPE_RAY: {

      pipeline_create_ray(pipeline);

      pipeline_create_sbt_buffer(pipeline);

      break;
    }
    case PIPELINE_TYPE_COMP: {

      pipeline_create_comp(pipeline);

      break;
    }
  }
}
void pipeline_destroy(pipeline_t *pipeline) {
  switch (pipeline->pipeline_type) {
    case PIPELINE_TYPE_RAY: {

      pipeline_destroy_sbt_buffer(pipeline);

      break;
    }
  }

  vkDestroyDescriptorPool(g_window.device, pipeline->descriptor_pool, 0);
  vkDestroyDescriptorSetLayout(g_window.device, pipeline->descriptor_set_layout_base, 0);
  vkDestroyPipelineLayout(g_window.device, pipeline->pipeline_layout, 0);
  vkDestroyPipeline(g_window.device, pipeline->pipeline_handle, 0);

  HEAP_FREE(pipeline->descriptor_set_layout);
  HEAP_FREE(pipeline->descriptor_set);
}

static void pipeline_create_descriptor_pool(pipeline_t *pipeline) {
  uint32_t descriptor_pool_index = 0;
  uint32_t descriptor_pool_count = pipeline->descriptor_pool_size_count;

  while (descriptor_pool_index < descriptor_pool_count) {

    pipeline->descriptor_pool_size[descriptor_pool_index].descriptorCount *= pipeline->descriptor_set_count;

    descriptor_pool_index++;
  }

  VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
    .pPoolSizes = pipeline->descriptor_pool_size,
    .poolSizeCount = pipeline->descriptor_pool_size_count,
    .maxSets = pipeline->descriptor_set_count,
  };

  VK_CHECK(vkCreateDescriptorPool(g_window.device, &descriptor_pool_create_info, 0, &pipeline->descriptor_pool));
}
static void pipeline_create_descriptor_set_layout(pipeline_t *pipeline) {
  VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
    .pBindings = pipeline->descriptor_set_layout_binding,
    .bindingCount = pipeline->descriptor_set_layout_binding_count,
    .pNext = 0,
  };

  VK_CHECK(vkCreateDescriptorSetLayout(g_window.device, &descriptor_set_layout_create_info, 0, &pipeline->descriptor_set_layout_base));
}
static void pipeline_create_descriptor_set(pipeline_t *pipeline) {
  uint32_t descriptor_set_index = 0;
  uint32_t descriptor_set_count = pipeline->descriptor_set_count;

  while (descriptor_set_index < descriptor_set_count) {

    pipeline->descriptor_set_layout[descriptor_set_index] = pipeline->descriptor_set_layout_base;

    descriptor_set_index++;
  }

  VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
    .descriptorSetCount = pipeline->descriptor_set_count,
    .descriptorPool = pipeline->descriptor_pool,
    .pSetLayouts = pipeline->descriptor_set_layout,
  };

  VK_CHECK(vkAllocateDescriptorSets(g_window.device, &descriptor_set_allocate_info, pipeline->descriptor_set));
}
static void pipeline_create_pipeline_layout(pipeline_t *pipeline) {
  VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .setLayoutCount = 1,
    .pSetLayouts = &pipeline->descriptor_set_layout_base,
    .pPushConstantRanges = pipeline->push_constant_range,
    .pushConstantRangeCount = pipeline->push_constant_range_count,
  };

  VK_CHECK(vkCreatePipelineLayout(g_window.device, &pipeline_layout_create_info, 0, &pipeline->pipeline_layout));
}

static void pipeline_create_sbt_buffer(pipeline_t *pipeline) {
  pipeline->ray_gen_group_count = 1;
  pipeline->ray_miss_group_count = 1;
  pipeline->ray_hit_group_count = 1;
  pipeline->callable_group_count = 0;

  uint32_t handle_size = g_physical_device_ray_tracing_pipeline_properties.shaderGroupHandleSize;
  uint32_t handle_alignment = g_physical_device_ray_tracing_pipeline_properties.shaderGroupHandleAlignment;
  uint32_t base_alignment = g_physical_device_ray_tracing_pipeline_properties.shaderGroupBaseAlignment;

  uint32_t aligned_handle_size = ALIGN_UP_BY(handle_size, handle_alignment);

  uint64_t ray_gen_size = aligned_handle_size * pipeline->ray_gen_group_count;
  uint64_t ray_miss_size = aligned_handle_size * pipeline->ray_miss_group_count;
  uint64_t ray_hit_size = aligned_handle_size * pipeline->ray_hit_group_count;
  uint64_t callable_size = 0;

  uint64_t ray_gen_region_size = ALIGN_UP_BY(ray_gen_size, base_alignment);
  uint64_t ray_miss_region_size = ALIGN_UP_BY(ray_miss_size, base_alignment);
  uint64_t ray_hit_region_size = ALIGN_UP_BY(ray_hit_size, base_alignment);
  uint64_t callable_region_size = ALIGN_UP_BY(callable_size, base_alignment);

  uint64_t sbt_buffer_size = ray_gen_region_size + ray_miss_region_size + ray_hit_region_size + callable_region_size;

  VkBufferCreateInfo buffer_create_info = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    .size = sbt_buffer_size,
    .usage = VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
  };

  VK_CHECK(vkCreateBuffer(g_window.device, &buffer_create_info, 0, &pipeline->sbt_buffer_handle));

  VkMemoryRequirements memory_requirements = {0};

  vkGetBufferMemoryRequirements(g_window.device, pipeline->sbt_buffer_handle, &memory_requirements);

  uint32_t memory_type_index = vkutil_find_memory_type_index(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  VkMemoryAllocateFlagsInfo memory_allocate_flags_info = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
    .flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
  };

  VkMemoryAllocateInfo memory_allocate_info = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
    .pNext = &memory_allocate_flags_info,
    .allocationSize = memory_requirements.size,
    .memoryTypeIndex = memory_type_index,
  };

  VK_CHECK(vkAllocateMemory(g_window.device, &memory_allocate_info, 0, &pipeline->sbt_device_memory));
  VK_CHECK(vkBindBufferMemory(g_window.device, pipeline->sbt_buffer_handle, pipeline->sbt_device_memory, 0));

  uint8_t *sbt_device_data = 0;

  VK_CHECK(vkMapMemory(g_window.device, pipeline->sbt_device_memory, 0, sbt_buffer_size, 0, &sbt_device_data));

  uint8_t *handles = (uint8_t *)HEAP_ALLOC(handle_size * 3, 0, 0);

  VK_CHECK(vkGetRayTracingShaderGroupHandlesKHR_proc(g_window.device, pipeline->pipeline_handle, 0, 3, handle_size * 3, handles));

  memcpy(sbt_device_data, handles + handle_size * 0, handle_size);
  sbt_device_data += ray_gen_region_size;
  memcpy(sbt_device_data, handles + handle_size * 1, handle_size);
  sbt_device_data += ray_miss_region_size;
  memcpy(sbt_device_data, handles + handle_size * 2, handle_size);

  HEAP_FREE(handles);

  vkUnmapMemory(g_window.device, pipeline->sbt_device_memory);

  VkBufferDeviceAddressInfo buffer_device_address_info = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
    .buffer = pipeline->sbt_buffer_handle,
  };

  pipeline->sbt_device_address = vkGetBufferDeviceAddress(g_window.device, &buffer_device_address_info);

  pipeline->ray_gen_region.deviceAddress = pipeline->sbt_device_address;
  pipeline->ray_gen_region.stride = aligned_handle_size;
  pipeline->ray_gen_region.size = aligned_handle_size;

  pipeline->ray_miss_region.deviceAddress = ALIGN_UP_BY(pipeline->sbt_device_address + aligned_handle_size, base_alignment);
  pipeline->ray_miss_region.stride = aligned_handle_size;
  pipeline->ray_miss_region.size = ray_miss_region_size;

  pipeline->ray_hit_region.deviceAddress = ALIGN_UP_BY(pipeline->sbt_device_address + aligned_handle_size, base_alignment) + ray_miss_region_size;
  pipeline->ray_hit_region.stride = aligned_handle_size;
  pipeline->ray_hit_region.size = ray_hit_region_size;

  pipeline->callable_region.deviceAddress = 0;
  pipeline->callable_region.stride = 0;
  pipeline->callable_region.size = 0;
}

static void pipeline_create_dflt(pipeline_t *pipeline) {
  VkShaderModule vertex_module = 0;
  VkShaderModule fragment_module = 0;

  {
    uint8_t *shader_bytes = 0;
    uint64_t shader_size = 0;

    fsutil_load_binary(&shader_bytes, &shader_size, pipeline->vertex_shader);

    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = (uint32_t const *)shader_bytes,
      .codeSize = shader_size,
    };

    VK_CHECK(vkCreateShaderModule(g_window.device, &shader_module_create_info, 0, &vertex_module));

    HEAP_FREE(shader_bytes);
  }

  {
    uint8_t *shader_bytes = 0;
    uint64_t shader_size = 0;

    fsutil_load_binary(&shader_bytes, &shader_size, pipeline->fragment_shader);

    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = (uint32_t const *)shader_bytes,
      .codeSize = shader_size,
    };

    VK_CHECK(vkCreateShaderModule(g_window.device, &shader_module_create_info, 0, &fragment_module));

    HEAP_FREE(shader_bytes);
  }

  VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info[] = {
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .module = vertex_module,
      .pName = "main",
    },
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = fragment_module,
      .pName = "main",
    },
  };

  VkPipelineVertexInputStateCreateInfo pipeline_vertex_input_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    .pVertexBindingDescriptions = pipeline->vertex_input_binding_description,
    .vertexBindingDescriptionCount = pipeline->vertex_input_binding_description_count,
    .pVertexAttributeDescriptions = pipeline->vertex_input_attribute_description,
    .vertexAttributeDescriptionCount = pipeline->vertex_input_attribute_description_count,
  };

  VkPipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    .topology = pipeline->primitive_topology,
    .primitiveRestartEnable = 0,
  };

  VkViewport viewport = {
    .x = 0.0F,
    .y = 0.0F,
    .width = (float)g_window.window_width,
    .height = (float)g_window.window_height,
    .minDepth = 0.0F,
    .maxDepth = 1.0F,
  };

  VkRect2D scissor = {
    .offset.x = 0,
    .offset.y = 0,
    .extent = {
      .width = g_window.window_width,
      .height = g_window.window_height,
    },
  };

  VkPipelineViewportStateCreateInfo pipeline_viewport_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .viewportCount = 1,
    .pViewports = &viewport,
    .scissorCount = 1,
    .pScissors = &scissor,
  };

  VkPipelineRasterizationStateCreateInfo pipeline_rasterization_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    .depthClampEnable = 0,
    .rasterizerDiscardEnable = 0,
    .polygonMode = pipeline->polygon_mode,
    .lineWidth = 1.0F,
    .cullMode = pipeline->cull_mode,
    .frontFace = VK_FRONT_FACE_CLOCKWISE,
    .depthBiasEnable = 0,
    .depthBiasConstantFactor = 0.0F,
    .depthBiasClamp = 0.0F,
    .depthBiasSlopeFactor = 0.0F,
  };

  VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    .sampleShadingEnable = 0,
    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    .minSampleShading = 1.0F,
    .pSampleMask = 0,
    .alphaToCoverageEnable = 0,
    .alphaToOneEnable = 0,
  };

  VkPipelineColorBlendAttachmentState pipeline_color_blend_attachment_state = {
    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    .blendEnable = pipeline->enable_blending,
    .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
    .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .colorBlendOp = VK_BLEND_OP_ADD,
    .srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .alphaBlendOp = VK_BLEND_OP_ADD,
  };

  VkPipelineDepthStencilStateCreateInfo pipeline_depth_stencil_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
    .depthTestEnable = pipeline->enable_depth_test,
    .depthWriteEnable = pipeline->enable_depth_write,
    .depthCompareOp = VK_COMPARE_OP_LESS,
    .depthBoundsTestEnable = 0,
    .stencilTestEnable = 0,
  };

  VkPipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .logicOpEnable = 0,
    .logicOp = VK_LOGIC_OP_COPY,
    .attachmentCount = 1,
    .pAttachments = &pipeline_color_blend_attachment_state,
    .blendConstants = {
      0.0F,
      0.0F,
      0.0F,
      0.0F,
    },
  };

  VkDynamicState dynamic_state[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR,
  };

  VkPipelineDynamicStateCreateInfo pipeline_dynamic_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
    .pDynamicStates = dynamic_state,
    .dynamicStateCount = ARRAY_COUNT(dynamic_state),
  };

  VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .pStages = pipeline_shader_stage_create_info,
    .stageCount = ARRAY_COUNT(pipeline_shader_stage_create_info),
    .pVertexInputState = &pipeline_vertex_input_state_create_info,
    .pInputAssemblyState = &pipeline_input_assembly_state_create_info,
    .pViewportState = &pipeline_viewport_state_create_info,
    .pRasterizationState = &pipeline_rasterization_state_create_info,
    .pMultisampleState = &pipeline_multisample_state_create_info,
    .pDepthStencilState = &pipeline_depth_stencil_state_create_info,
    .pColorBlendState = &pipeline_color_blend_state_create_info,
    .pDynamicState = &pipeline_dynamic_state_create_info,
    .layout = pipeline->pipeline_layout,
    .renderPass = *pipeline->render_pass,
    .subpass = 0,
    .basePipelineHandle = 0,
  };

  VK_CHECK(vkCreateGraphicsPipelines(g_window.device, 0, 1, &graphics_pipeline_create_info, 0, &pipeline->pipeline_handle));

  vkDestroyShaderModule(g_window.device, vertex_module, 0);
  vkDestroyShaderModule(g_window.device, fragment_module, 0);
}
static void pipeline_create_mesh(pipeline_t *pipeline) {
  VkShaderModule task_module = 0;
  VkShaderModule mesh_module = 0;
  VkShaderModule fragment_module = 0;

  {
    uint8_t *shader_bytes = 0;
    uint64_t shader_size = 0;

    fsutil_load_binary(&shader_bytes, &shader_size, pipeline->task_shader);

    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = (uint32_t const *)shader_bytes,
      .codeSize = shader_size,
    };

    VK_CHECK(vkCreateShaderModule(g_window.device, &shader_module_create_info, 0, &task_module));

    HEAP_FREE(shader_bytes);
  }

  {
    uint8_t *shader_bytes = 0;
    uint64_t shader_size = 0;

    fsutil_load_binary(&shader_bytes, &shader_size, pipeline->mesh_shader);

    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = (uint32_t const *)shader_bytes,
      .codeSize = shader_size,
    };

    VK_CHECK(vkCreateShaderModule(g_window.device, &shader_module_create_info, 0, &mesh_module));

    HEAP_FREE(shader_bytes);
  }

  {
    uint8_t *shader_bytes = 0;
    uint64_t shader_size = 0;

    fsutil_load_binary(&shader_bytes, &shader_size, pipeline->fragment_shader);

    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = (uint32_t const *)shader_bytes,
      .codeSize = shader_size,
    };

    VK_CHECK(vkCreateShaderModule(g_window.device, &shader_module_create_info, 0, &fragment_module));

    HEAP_FREE(shader_bytes);
  }

  VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info[] = {
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_TASK_BIT_EXT,
      .module = task_module,
      .pName = "main",
    },
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_MESH_BIT_EXT,
      .module = mesh_module,
      .pName = "main",
    },
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = fragment_module,
      .pName = "main",
    },
  };

  VkViewport viewport = {
    .x = 0.0F,
    .y = 0.0F,
    .width = (float)g_window.window_width,
    .height = (float)g_window.window_height,
    .minDepth = 0.0F,
    .maxDepth = 1.0F,
  };

  VkRect2D scissor = {
    .offset.x = 0,
    .offset.y = 0,
    .extent = {
      .width = g_window.window_width,
      .height = g_window.window_height,
    },
  };

  VkPipelineViewportStateCreateInfo pipeline_viewport_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .viewportCount = 1,
    .pViewports = &viewport,
    .scissorCount = 1,
    .pScissors = &scissor,
  };

  VkPipelineRasterizationStateCreateInfo pipeline_rasterization_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    .depthClampEnable = 0,
    .rasterizerDiscardEnable = 0,
    .polygonMode = pipeline->polygon_mode,
    .lineWidth = 1.0F,
    .cullMode = pipeline->cull_mode,
    .frontFace = VK_FRONT_FACE_CLOCKWISE,
    .depthBiasEnable = 0,
    .depthBiasConstantFactor = 0.0F,
    .depthBiasClamp = 0.0F,
    .depthBiasSlopeFactor = 0.0F,
  };

  VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    .sampleShadingEnable = 0,
    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    .minSampleShading = 1.0F,
    .pSampleMask = 0,
    .alphaToCoverageEnable = 0,
    .alphaToOneEnable = 0,
  };

  VkPipelineColorBlendAttachmentState pipeline_color_blend_attachment_state = {
    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    .blendEnable = pipeline->enable_blending,
    .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
    .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .colorBlendOp = VK_BLEND_OP_ADD,
    .srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .alphaBlendOp = VK_BLEND_OP_ADD,
  };

  VkPipelineDepthStencilStateCreateInfo pipeline_depth_stencil_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
    .depthTestEnable = pipeline->enable_depth_test,
    .depthWriteEnable = pipeline->enable_depth_write,
    .depthCompareOp = VK_COMPARE_OP_LESS,
    .depthBoundsTestEnable = 0,
    .stencilTestEnable = 0,
  };

  VkPipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .logicOpEnable = 0,
    .logicOp = VK_LOGIC_OP_COPY,
    .attachmentCount = 1,
    .pAttachments = &pipeline_color_blend_attachment_state,
    .blendConstants = {
      0.0F,
      0.0F,
      0.0F,
      0.0F,
    },
  };

  VkDynamicState dynamic_state[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR,
  };

  VkPipelineDynamicStateCreateInfo pipeline_dynamic_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
    .pDynamicStates = dynamic_state,
    .dynamicStateCount = ARRAY_COUNT(dynamic_state),
  };

  VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .pStages = pipeline_shader_stage_create_info,
    .stageCount = ARRAY_COUNT(pipeline_shader_stage_create_info),
    .pVertexInputState = 0,
    .pInputAssemblyState = 0,
    .pViewportState = &pipeline_viewport_state_create_info,
    .pRasterizationState = &pipeline_rasterization_state_create_info,
    .pMultisampleState = &pipeline_multisample_state_create_info,
    .pDepthStencilState = &pipeline_depth_stencil_state_create_info,
    .pColorBlendState = &pipeline_color_blend_state_create_info,
    .pDynamicState = &pipeline_dynamic_state_create_info,
    .layout = pipeline->pipeline_layout,
    .renderPass = *pipeline->render_pass,
    .subpass = 0,
    .basePipelineHandle = 0,
  };

  VK_CHECK(vkCreateGraphicsPipelines(g_window.device, 0, 1, &graphics_pipeline_create_info, 0, &pipeline->pipeline_handle));

  vkDestroyShaderModule(g_window.device, task_module, 0);
  vkDestroyShaderModule(g_window.device, mesh_module, 0);
  vkDestroyShaderModule(g_window.device, fragment_module, 0);
}
static void pipeline_create_ray(pipeline_t *pipeline) {
  VkShaderModule ray_gen_module = 0;
  VkShaderModule ray_miss_module = 0;
  VkShaderModule ray_intersect_module = 0;
  VkShaderModule ray_closest_hit_module = 0;

  {
    uint8_t *shader_bytes = 0;
    uint64_t shader_size = 0;

    fsutil_load_binary(&shader_bytes, &shader_size, pipeline->ray_gen_shader);

    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = (uint32_t const *)shader_bytes,
      .codeSize = shader_size,
    };

    VK_CHECK(vkCreateShaderModule(g_window.device, &shader_module_create_info, 0, &ray_gen_module));

    HEAP_FREE(shader_bytes);
  }

  {
    uint8_t *shader_bytes = 0;
    uint64_t shader_size = 0;

    fsutil_load_binary(&shader_bytes, &shader_size, pipeline->ray_miss_shader);

    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = (uint32_t const *)shader_bytes,
      .codeSize = shader_size,
    };

    VK_CHECK(vkCreateShaderModule(g_window.device, &shader_module_create_info, 0, &ray_miss_module));

    HEAP_FREE(shader_bytes);
  }

  {
    uint8_t *shader_bytes = 0;
    uint64_t shader_size = 0;

    fsutil_load_binary(&shader_bytes, &shader_size, pipeline->ray_intersect_shader);

    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = (uint32_t const *)shader_bytes,
      .codeSize = shader_size,
    };

    VK_CHECK(vkCreateShaderModule(g_window.device, &shader_module_create_info, 0, &ray_intersect_module));

    HEAP_FREE(shader_bytes);
  }

  {
    uint8_t *shader_bytes = 0;
    uint64_t shader_size = 0;

    fsutil_load_binary(&shader_bytes, &shader_size, pipeline->ray_closest_hit_shader);

    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = (uint32_t const *)shader_bytes,
      .codeSize = shader_size,
    };

    VK_CHECK(vkCreateShaderModule(g_window.device, &shader_module_create_info, 0, &ray_closest_hit_module));

    HEAP_FREE(shader_bytes);
  }

  VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info[] = {
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR,
      .module = ray_gen_module,
      .pName = "main",
    },
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_MISS_BIT_KHR,
      .module = ray_miss_module,
      .pName = "main",
    },
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_INTERSECTION_BIT_KHR,
      .module = ray_intersect_module,
      .pName = "main",
    },
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR,
      .module = ray_closest_hit_module,
      .pName = "main",
    },
  };

  VkRayTracingShaderGroupCreateInfoKHR ray_tracing_shader_group_create_info[] = {
    {
      .sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR,
      .type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR,
      .generalShader = 0,
      .closestHitShader = VK_SHADER_UNUSED_KHR,
      .anyHitShader = VK_SHADER_UNUSED_KHR,
      .intersectionShader = VK_SHADER_UNUSED_KHR,
    },
    {
      .sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR,
      .type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR,
      .generalShader = 1,
      .closestHitShader = VK_SHADER_UNUSED_KHR,
      .anyHitShader = VK_SHADER_UNUSED_KHR,
      .intersectionShader = VK_SHADER_UNUSED_KHR,
    },
    {
      .sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR,
      .type = VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_KHR,
      .generalShader = VK_SHADER_UNUSED_KHR,
      .closestHitShader = 3,
      .anyHitShader = VK_SHADER_UNUSED_KHR,
      .intersectionShader = 2,
    },
  };

  VkRayTracingPipelineCreateInfoKHR ray_tracing_pipeline_create_info = {
    .sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR,
    .pStages = pipeline_shader_stage_create_info,
    .stageCount = ARRAY_COUNT(pipeline_shader_stage_create_info),
    .pGroups = ray_tracing_shader_group_create_info,
    .groupCount = ARRAY_COUNT(ray_tracing_shader_group_create_info),
    .maxPipelineRayRecursionDepth = 1,
    .pLibraryInfo = 0,
    .pLibraryInterface = 0,
    .pDynamicState = 0,
    .layout = pipeline->pipeline_layout,
    .basePipelineHandle = 0,
    .basePipelineIndex = 0,
  };

  VK_CHECK(vkCreateRayTracingPipelinesKHR_proc(g_window.device, 0, 0, 1, &ray_tracing_pipeline_create_info, 0, &pipeline->pipeline_handle));

  vkDestroyShaderModule(g_window.device, ray_gen_module, 0);
  vkDestroyShaderModule(g_window.device, ray_miss_module, 0);
  vkDestroyShaderModule(g_window.device, ray_intersect_module, 0);
  vkDestroyShaderModule(g_window.device, ray_closest_hit_module, 0);
}
static void pipeline_create_comp(pipeline_t *pipeline) {
  VkShaderModule compute_module = 0;

  {
    uint8_t *shader_bytes = 0;
    uint64_t shader_size = 0;

    fsutil_load_binary(&shader_bytes, &shader_size, pipeline->compute_shader);

    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = (uint32_t const *)shader_bytes,
      .codeSize = shader_size,
    };

    VK_CHECK(vkCreateShaderModule(g_window.device, &shader_module_create_info, 0, &compute_module));

    HEAP_FREE(shader_bytes);
  }

  VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    .stage = VK_SHADER_STAGE_COMPUTE_BIT,
    .module = compute_module,
    .pName = "main",
  };

  VkComputePipelineCreateInfo compute_pipeline_create_info = {
    .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
    .layout = pipeline->pipeline_layout,
    .stage = pipeline_shader_stage_create_info,
  };

  VK_CHECK(vkCreateComputePipelines(g_window.device, 0, 1, &compute_pipeline_create_info, 0, &pipeline->pipeline_handle));

  vkDestroyShaderModule(g_window.device, compute_module, 0);
}

static void pipeline_destroy_sbt_buffer(pipeline_t *pipeline) {
  vkFreeMemory(g_window.device, pipeline->sbt_device_memory, 0);
  vkDestroyBuffer(g_window.device, pipeline->sbt_buffer_handle, 0);
}
