#include <pch.h>

static void pipeline_create_descriptor_pool(pipeline_t *pipeline);
static void pipeline_create_descriptor_set_layout(pipeline_t *pipeline);
static void pipeline_create_descriptor_set(pipeline_t *pipeline);
static void pipeline_create_pipeline_layout(pipeline_t *pipeline);

static void pipeline_create_vf(pipeline_t *pipeline);
static void pipeline_create_tmf(pipeline_t *pipeline);
static void pipeline_create_c(pipeline_t *pipeline);

void pipeline_create(pipeline_t *pipeline) {
  pipeline_create_descriptor_pool(pipeline);
  pipeline_create_descriptor_set_layout(pipeline);
  pipeline_create_descriptor_set(pipeline);
  pipeline_create_pipeline_layout(pipeline);

  switch (pipeline->pipeline_type) {
    case PIPELINE_TYPE_VF: {

      pipeline_create_vf(pipeline);

      break;
    }
    case PIPELINE_TYPE_TMF: {

      pipeline_create_tmf(pipeline);

      break;
    }
    case PIPELINE_TYPE_C: {

      pipeline_create_c(pipeline);

      break;
    }
  }
}
void pipeline_destroy(pipeline_t *pipeline) {
  vkDestroyDescriptorPool(g_window.device, pipeline->descriptor_pool, 0);
  vkDestroyDescriptorSetLayout(g_window.device, pipeline->descriptor_set_layout, 0);
  vkDestroyPipelineLayout(g_window.device, pipeline->pipeline_layout, 0);
  vkDestroyPipeline(g_window.device, pipeline->pipeline_handle, 0);
}

static void pipeline_create_descriptor_pool(pipeline_t *pipeline) {
  VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
    .pPoolSizes = pipeline->descriptor_pool_size,
    .poolSizeCount = pipeline->descriptor_pool_size_count,
    .maxSets = 1,
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

  VK_CHECK(vkCreateDescriptorSetLayout(g_window.device, &descriptor_set_layout_create_info, 0, &pipeline->descriptor_set_layout));
}
static void pipeline_create_descriptor_set(pipeline_t *pipeline) {
  VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
    .descriptorSetCount = 1,
    .descriptorPool = pipeline->descriptor_pool,
    .pSetLayouts = &pipeline->descriptor_set_layout,
  };

  VK_CHECK(vkAllocateDescriptorSets(g_window.device, &descriptor_set_allocate_info, &pipeline->descriptor_set));
}
static void pipeline_create_pipeline_layout(pipeline_t *pipeline) {
  VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .setLayoutCount = 1,
    .pSetLayouts = &pipeline->descriptor_set_layout,
    .pPushConstantRanges = 0,
    .pushConstantRangeCount = 0,
  };

  VK_CHECK(vkCreatePipelineLayout(g_window.device, &pipeline_layout_create_info, 0, &pipeline->pipeline_layout));
}

static void pipeline_create_vf(pipeline_t *pipeline) {
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
    .topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
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
    .polygonMode = VK_POLYGON_MODE_FILL,
    .lineWidth = 1.0F,
    .cullMode = VK_CULL_MODE_BACK_BIT,
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
    .blendEnable = 1,
    .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
    .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .colorBlendOp = VK_BLEND_OP_ADD,
    .srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .alphaBlendOp = VK_BLEND_OP_ADD,
  };

  VkPipelineDepthStencilStateCreateInfo pipeline_depth_stencil_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
    .depthTestEnable = 0,
    .depthWriteEnable = 0,
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
static void pipeline_create_tmf(pipeline_t *pipeline) {
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
    .polygonMode = VK_POLYGON_MODE_FILL,
    .lineWidth = 1.0F,
    .cullMode = VK_CULL_MODE_BACK_BIT,
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
    .blendEnable = 0,
    .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
    .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .colorBlendOp = VK_BLEND_OP_ADD,
    .srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .alphaBlendOp = VK_BLEND_OP_ADD,
  };

  VkPipelineDepthStencilStateCreateInfo pipeline_depth_stencil_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
    .depthTestEnable = 1,
    .depthWriteEnable = 1,
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
static void pipeline_create_c(pipeline_t *pipeline) {
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
