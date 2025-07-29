#include "Rendering/Pipeline/Pipeline.h"
#include "Rendering/Renderer.h"
#include "Rendering/Utils.h"

namespace yk
{

  PipelineShaders::~PipelineShaders()
  {
    for (auto& specInfo : m_SpecializationInfos)
      if (specInfo.pData)
        delete specInfo.pData;
  }

  void PipelineShaders::AddShader(const std::shared_ptr<Shader>& shader, const char* entry_point)
  {
    YK_ASSERT(PipelineShaders::CheckForShaderCount(shader->GetType()), "[SYSTEM/VULKAN] Can't have multiple shaders of the same type in a single pipeline");

    VkPipelineShaderStageCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.module = shader->GetModule();
    createInfo.pName = entry_point;
    createInfo.stage = PipelineShaders::StageFromShaderType(shader->GetType());
    createInfo.pSpecializationInfo = VK_NULL_HANDLE;

    m_ShaderStages.push_back(createInfo);
  }

  bool PipelineShaders::CheckForShaderCount(Shader::Type type)
  {
    for (const VkPipelineShaderStageCreateInfo& createInfo : m_ShaderStages)
      if (createInfo.stage == PipelineShaders::StageFromShaderType(type))
        return false;
    return true;
  }

  VkShaderStageFlagBits PipelineShaders::StageFromShaderType(Shader::Type type)
  {
    switch (type)
    {
      case Shader::Type::Vertex:                  return VK_SHADER_STAGE_VERTEX_BIT;
      case Shader::Type::TessellationControl:     return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
      case Shader::Type::TessellationEvaluation:  return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
      case Shader::Type::Geometry:                return VK_SHADER_STAGE_GEOMETRY_BIT;
      case Shader::Type::Fragment:                return VK_SHADER_STAGE_FRAGMENT_BIT;
      case Shader::Type::Compute:                 return VK_SHADER_STAGE_COMPUTE_BIT;
      case Shader::Type::RayGeneration:           return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
      case Shader::Type::AnyHit:                  return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
      case Shader::Type::ClosestHit:              return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
      case Shader::Type::Miss:                    return VK_SHADER_STAGE_MISS_BIT_KHR;
      case Shader::Type::Intersection:            return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
      case Shader::Type::Callable:                return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
      case Shader::Type::Task:                    return VK_SHADER_STAGE_TASK_BIT_EXT;
      case Shader::Type::Mesh:                    return VK_SHADER_STAGE_MESH_BIT_EXT;
      default:                                    return VK_SHADER_STAGE_VERTEX_BIT;
    }
  }

  void PipelineVertexInputState::AddBinding(uint32_t binding, uint32_t stride, InputRate input_rate)
  {
    VkVertexInputRate vkrate = VK_VERTEX_INPUT_RATE_MAX_ENUM;
    switch (input_rate)
    {
      case InputRate::Vertex:   vkrate = VK_VERTEX_INPUT_RATE_VERTEX; break;
      case InputRate::Instance: vkrate = VK_VERTEX_INPUT_RATE_INSTANCE; break;
    }

    m_VertexInputBindings.push_back({ binding, stride, vkrate });
  }

  void PipelineVertexInputState::AddAttribute(uint32_t location, VertexInputFormat format, uint32_t offset)
  {
    VkFormat vkformat = VK_FORMAT_UNDEFINED;
    switch (format)
    {
      case VertexInputFormat::SINT:  vkformat = VK_FORMAT_R32_SINT; break;
      case VertexInputFormat::SVEC2: vkformat = VK_FORMAT_R32G32_SINT; break;
      case VertexInputFormat::SVEC3: vkformat = VK_FORMAT_R32G32B32_SINT; break;
      case VertexInputFormat::SVEC4: vkformat = VK_FORMAT_R32G32B32A32_SINT; break;
      case VertexInputFormat::UINT:  vkformat = VK_FORMAT_R32_UINT; break;
      case VertexInputFormat::UVEC2: vkformat = VK_FORMAT_R32G32_UINT; break;
      case VertexInputFormat::UVEC3: vkformat = VK_FORMAT_R32G32B32_UINT; break;
      case VertexInputFormat::UVEC4: vkformat = VK_FORMAT_R32G32B32A32_UINT; break;
      case VertexInputFormat::FLOAT: vkformat = VK_FORMAT_R32_SFLOAT; break;
      case VertexInputFormat::VEC2:  vkformat = VK_FORMAT_R32G32_SFLOAT; break;
      case VertexInputFormat::VEC3:  vkformat = VK_FORMAT_R32G32B32_SFLOAT; break;
      case VertexInputFormat::VEC4:  vkformat = VK_FORMAT_R32G32B32A32_SFLOAT; break;
    }

    m_VertexInputAttributes.push_back({ location, m_VertexInputBindings.back().binding, vkformat, offset });
  }

  VkPipelineVertexInputStateCreateInfo PipelineVertexInputState::Get() const
  {
    VkPipelineVertexInputStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    createInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(m_VertexInputBindings.size());
    createInfo.pVertexBindingDescriptions = m_VertexInputBindings.data();
    createInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_VertexInputAttributes.size());
    createInfo.pVertexAttributeDescriptions = m_VertexInputAttributes.data();

    return createInfo;
  }

  PipelineInputAssemblyState::PipelineInputAssemblyState(InputPrimitiveTopology topology, bool reset_primitive)
  {
    YK_ASSERT(PipelineInputAssemblyState::CheckResetPrimitiveToTopology(topology, reset_primitive), "[SYSTEM/VULKAN] Cannot reset primitives with this input primitive topology");

    VkPrimitiveTopology vktopology = VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
    switch (topology)
    {
      case InputPrimitiveTopology::PointList:                   vktopology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST; break;
      case InputPrimitiveTopology::LineList:                    vktopology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST; break;
      case InputPrimitiveTopology::LineListWithAdjacency:       vktopology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY; break;
      case InputPrimitiveTopology::LineStrip:                   vktopology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP; break;
      case InputPrimitiveTopology::LineStripWithAdjacency:      vktopology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY; break;
      case InputPrimitiveTopology::TriangleList:                vktopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; break;
      case InputPrimitiveTopology::TriangleListWithAdjacency:   vktopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY; break;
      case InputPrimitiveTopology::TriangleStrip:               vktopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP; break;
      case InputPrimitiveTopology::TriangleStripWithAdjacency:  vktopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY; break;
      case InputPrimitiveTopology::TriangleFan:                 vktopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN; break; 
      case InputPrimitiveTopology::PatchList:                   vktopology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST; break;
    }

    m_InputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    m_InputAssemblyState.topology = vktopology;
    m_InputAssemblyState.primitiveRestartEnable = reset_primitive ? VK_TRUE : VK_FALSE;
  }

  bool PipelineInputAssemblyState::CheckResetPrimitiveToTopology(InputPrimitiveTopology topology, bool reset_primitive)
  {
    switch (topology)
    {
      case InputPrimitiveTopology::PointList:
      case InputPrimitiveTopology::LineList:
      case InputPrimitiveTopology::LineListWithAdjacency:
      case InputPrimitiveTopology::TriangleList:
      case InputPrimitiveTopology::TriangleListWithAdjacency:
      case InputPrimitiveTopology::PatchList:    
      {
        if (reset_primitive) 
          return false; 
        else 
          return true;
      }
      default:
        return true;
    }
  }

  PipelineTessellationState::PipelineTessellationState(uint32_t patch_control_points)
  {
    m_TessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    m_TessellationState.patchControlPoints = patch_control_points;
  }

  void PipelineViewportScissorState::AddViewport(Viewport viewport)
  {
    m_Viewports.push_back(viewport);
    m_Scissors.push_back({ {0, 0}, {static_cast<uint32_t>(viewport.width), static_cast<uint32_t>(viewport.height)} });
  }

  void PipelineViewportScissorState::AddViewport(Viewport viewport, Scissor scissor)
  {
    m_Viewports.push_back(viewport);
    m_Scissors.push_back(scissor);
  }

  VkPipelineViewportStateCreateInfo PipelineViewportScissorState::Get() const
  {
    VkPipelineViewportStateCreateInfo viewportScissorState = {};
    viewportScissorState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportScissorState.viewportCount = !m_Viewports.empty() ? static_cast<uint32_t>(m_Viewports.size()) : 1;
    viewportScissorState.pViewports = !m_Viewports.empty() ? m_Viewports.data() : VK_NULL_HANDLE;
    viewportScissorState.scissorCount = !m_Scissors.empty() ? static_cast<uint32_t>(m_Scissors.size()) : 1;
    viewportScissorState.pScissors = !m_Scissors.empty() ? m_Scissors.data() : VK_NULL_HANDLE;

    return viewportScissorState;
  }

  PipelineRasterizationState::PipelineRasterizationState(bool enable_rasterization, bool depth_clamp, PolygonMode polygon_mode, CullMode cull_mode, FrontFace front_face, float line_width)
  {
    m_RasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    m_RasterizationState.rasterizerDiscardEnable = enable_rasterization ? VK_FALSE : VK_TRUE;
    m_RasterizationState.depthClampEnable = depth_clamp ? VK_TRUE : VK_FALSE;

    {
      VkPolygonMode vkpolygoneMode;
      switch (polygon_mode)
      {
        case PolygonMode::Point: vkpolygoneMode = VK_POLYGON_MODE_POINT; break;
        case PolygonMode::Line:  vkpolygoneMode = VK_POLYGON_MODE_LINE; break;
        case PolygonMode::Fill:  vkpolygoneMode = VK_POLYGON_MODE_FILL; break;
      }
      m_RasterizationState.polygonMode = vkpolygoneMode;
    }

    {
      VkCullModeFlags vkcullMode;
      switch (cull_mode)
      {
        case CullMode::Front:         vkcullMode = VK_CULL_MODE_FRONT_BIT; break;
        case CullMode::Back:          vkcullMode = VK_CULL_MODE_BACK_BIT; break;
        case CullMode::FrontAndBack:  vkcullMode = VK_CULL_MODE_FRONT_AND_BACK; break;
      }
      m_RasterizationState.cullMode = vkcullMode;
    }

    {
      VkFrontFace vkfrontFace;
      switch (front_face)
      {
      case FrontFace::Clockwise:         vkfrontFace = VK_FRONT_FACE_CLOCKWISE; break;
      case FrontFace::CounterClockwise:  vkfrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; break;
      }
      m_RasterizationState.frontFace = vkfrontFace;
    }

    m_RasterizationState.lineWidth = line_width;
    m_RasterizationState.depthBiasEnable = VK_FALSE; 
  }

  PipelineRasterizationState::PipelineRasterizationState(bool enable_rasterization, bool depth_clamp, PolygonMode polygon_mode, CullMode cull_mode, FrontFace front_face, DepthBiasInfo depth_bias, float line_width)
   : PipelineRasterizationState(enable_rasterization, depth_clamp, polygon_mode, cull_mode, front_face, line_width)
  {
    m_RasterizationState.depthBiasEnable = VK_TRUE;
    m_RasterizationState.depthBiasConstantFactor = depth_bias.ConstantFactor;
    m_RasterizationState.depthBiasClamp = depth_bias.Clamp;
    m_RasterizationState.depthBiasSlopeFactor = depth_bias.SlopeFactor;
  }

  PipelineMultisampleState::PipelineMultisampleState(MultisampleCount count, bool enable_shading, float min_sample_shading, MultisampleMask sample_mask, bool alpha_to_coverage, bool alpha_to_one)
    : m_SampleMask(sample_mask)
  {
    m_MultisamplingState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

    {
      VkSampleCountFlagBits vkmultisampleCount;
      switch (count)
      {
      case MultisampleCount::_1:  vkmultisampleCount = VK_SAMPLE_COUNT_1_BIT; break;
      case MultisampleCount::_2:  vkmultisampleCount = VK_SAMPLE_COUNT_2_BIT; break;
      case MultisampleCount::_4:  vkmultisampleCount = VK_SAMPLE_COUNT_4_BIT; break;
      case MultisampleCount::_8:  vkmultisampleCount = VK_SAMPLE_COUNT_8_BIT; break;
      case MultisampleCount::_16: vkmultisampleCount = VK_SAMPLE_COUNT_16_BIT; break;
      case MultisampleCount::_32: vkmultisampleCount = VK_SAMPLE_COUNT_32_BIT; break;
      case MultisampleCount::_64: vkmultisampleCount = VK_SAMPLE_COUNT_64_BIT; break;
      }
      m_MultisamplingState.rasterizationSamples = vkmultisampleCount;
    }

    m_MultisamplingState.sampleShadingEnable = enable_shading ? VK_TRUE : VK_FALSE;
    m_MultisamplingState.minSampleShading = min_sample_shading;
    m_MultisamplingState.pSampleMask = sample_mask ? &m_SampleMask : VK_NULL_HANDLE;
    m_MultisamplingState.alphaToCoverageEnable = alpha_to_coverage ? VK_TRUE : VK_FALSE;
    m_MultisamplingState.alphaToOneEnable = alpha_to_one ? VK_TRUE : VK_FALSE;
  }

  PipelineMultisampleState::PipelineMultisampleState(VkSampleCountFlagBits count, bool enable_shading, float min_sample_shading, MultisampleMask sample_mask, bool alpha_to_coverage, bool alpha_to_one)
    : m_SampleMask(sample_mask)
  {
    m_MultisamplingState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    m_MultisamplingState.rasterizationSamples = count;
    m_MultisamplingState.sampleShadingEnable = enable_shading ? VK_TRUE : VK_FALSE;
    m_MultisamplingState.minSampleShading = min_sample_shading;
    m_MultisamplingState.pSampleMask = sample_mask ? &m_SampleMask : VK_NULL_HANDLE;
    m_MultisamplingState.alphaToCoverageEnable = alpha_to_coverage ? VK_TRUE : VK_FALSE;
    m_MultisamplingState.alphaToOneEnable = alpha_to_one ? VK_TRUE : VK_FALSE;
  }

  PipelineDepthStencilState::PipelineDepthStencilState()
  {
    m_DepthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    m_DepthStencilState.depthTestEnable = VK_FALSE;
    m_DepthStencilState.stencilTestEnable = VK_FALSE;
  }

  PipelineDepthStencilState::PipelineDepthStencilState(DepthTestInfo depth_test)
  {
    m_DepthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    m_DepthStencilState.depthTestEnable = VK_TRUE;
    m_DepthStencilState.depthWriteEnable = depth_test.EnableWrite ? VK_TRUE : VK_FALSE;

    {
      VkCompareOp vkcompareOp;
      switch (depth_test.CompareOp)
      {
        case CompareOp::Never:            vkcompareOp = VK_COMPARE_OP_NEVER; break;
        case CompareOp::Always:         vkcompareOp = VK_COMPARE_OP_ALWAYS; break;
        case CompareOp::Less:           vkcompareOp = VK_COMPARE_OP_LESS; break; break;
        case CompareOp::LessOrEqual:    vkcompareOp = VK_COMPARE_OP_LESS_OR_EQUAL; break;
        case CompareOp::Greater:        vkcompareOp = VK_COMPARE_OP_GREATER; break;
        case CompareOp::GreaterOrEqual: vkcompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL; break;
        case CompareOp::Equal:          vkcompareOp = VK_COMPARE_OP_EQUAL; break;
        case CompareOp::NotEqual:       vkcompareOp = VK_COMPARE_OP_NOT_EQUAL; break;
      }
      m_DepthStencilState.depthCompareOp = vkcompareOp;
    }

    m_DepthStencilState.depthBoundsTestEnable = depth_test.EnableBoundsTest ? VK_TRUE : VK_FALSE;
    m_DepthStencilState.minDepthBounds = depth_test.MinBounds;
    m_DepthStencilState.maxDepthBounds = depth_test.MaxBounds;
  }

  PipelineDepthStencilState::PipelineDepthStencilState(StencilTestInfo stencil_test)
  {
    m_DepthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    m_DepthStencilState.depthTestEnable = VK_FALSE;

    m_DepthStencilState.stencilTestEnable = VK_TRUE;
    m_DepthStencilState.front = stencil_test.FrontOpState;
    m_DepthStencilState.back = stencil_test.BackOpState;
  }

  PipelineDepthStencilState::PipelineDepthStencilState(DepthTestInfo depth_test, StencilTestInfo stencil_test)
  {
    m_DepthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    m_DepthStencilState.depthTestEnable = VK_TRUE;
    m_DepthStencilState.depthWriteEnable = depth_test.EnableWrite ? VK_TRUE : VK_FALSE;

    {
      VkCompareOp vkcompareOp;
      switch (depth_test.CompareOp)
      {
      case CompareOp::Never:          vkcompareOp = VK_COMPARE_OP_NEVER;
      case CompareOp::Always:         vkcompareOp = VK_COMPARE_OP_ALWAYS;
      case CompareOp::Less:           vkcompareOp = VK_COMPARE_OP_LESS;
      case CompareOp::LessOrEqual:    vkcompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
      case CompareOp::Greater:        vkcompareOp = VK_COMPARE_OP_GREATER;
      case CompareOp::GreaterOrEqual: vkcompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
      case CompareOp::Equal:          vkcompareOp = VK_COMPARE_OP_EQUAL;
      case CompareOp::NotEqual:       vkcompareOp = VK_COMPARE_OP_NOT_EQUAL;
      }
      m_DepthStencilState.depthCompareOp = vkcompareOp;
    }

    m_DepthStencilState.depthBoundsTestEnable = depth_test.EnableBoundsTest ? VK_TRUE : VK_FALSE;
    m_DepthStencilState.minDepthBounds = depth_test.MinBounds;
    m_DepthStencilState.maxDepthBounds = depth_test.MaxBounds;

    m_DepthStencilState.stencilTestEnable = VK_TRUE;
    m_DepthStencilState.front = stencil_test.FrontOpState;
    m_DepthStencilState.back = stencil_test.BackOpState;
  }

  PipelineColorBlendAttachments::PipelineColorBlendAttachments(const RenderPassData& data)
  {
    uint32_t colorAttachmentCount = 0;

    for (const VkSubpassDescription& subpass : data.Subpasses)
      colorAttachmentCount += subpass.colorAttachmentCount;

    m_States.reserve(colorAttachmentCount);
  }

  void PipelineColorBlendAttachments::AddAttachmentState(VkColorComponentFlags color_write_mask)
  {
    VkPipelineColorBlendAttachmentState state = {};
    state.blendEnable = VK_FALSE;
    state.colorWriteMask = color_write_mask;

    m_States.emplace_back(state);
  }

  void PipelineColorBlendAttachments::AddAttachmentState(VkBlendFactor src_color_blend_factor, VkBlendFactor dst_color_blend_factor, VkBlendOp color_blend_op, VkBlendFactor src_alpha_blend_factor, VkBlendFactor dst_alpha_blend_factor, VkBlendOp alpha_blend_op, VkColorComponentFlags color_write_mask)
  {
    VkPipelineColorBlendAttachmentState state = {};
    state.blendEnable = VK_TRUE;
    state.colorWriteMask = color_write_mask;

    state.srcColorBlendFactor = src_color_blend_factor;
    state.dstColorBlendFactor = dst_color_blend_factor;
    state.colorBlendOp = color_blend_op;

    state.srcAlphaBlendFactor = src_alpha_blend_factor;
    state.dstAlphaBlendFactor = dst_alpha_blend_factor;
    state.alphaBlendOp = alpha_blend_op;

    m_States.emplace_back(state);
  }

  PipelineColorBlendState::PipelineColorBlendState(LogicOp op)
  {
    m_ColorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

    m_ColorBlendState.logicOpEnable = VK_TRUE;

    {
      VkLogicOp vklogicOp;
      switch (op)
      {
        case LogicOp::Clear: vklogicOp = VK_LOGIC_OP_CLEAR; break;
        case LogicOp::And: vklogicOp = VK_LOGIC_OP_AND; break;
        case LogicOp::AndReverse: vklogicOp = VK_LOGIC_OP_AND_REVERSE; break;
        case LogicOp::Copy: vklogicOp = VK_LOGIC_OP_COPY; break;
        case LogicOp::AndInverted: vklogicOp = VK_LOGIC_OP_AND_INVERTED; break;
        case LogicOp::NoOp: vklogicOp = VK_LOGIC_OP_NO_OP; break;
        case LogicOp::XOr: vklogicOp = VK_LOGIC_OP_XOR; break;
        case LogicOp::Or: vklogicOp = VK_LOGIC_OP_OR; break;
        case LogicOp::NOr: vklogicOp = VK_LOGIC_OP_NOR; break;
        case LogicOp::Equivalent: vklogicOp = VK_LOGIC_OP_EQUIVALENT; break;
        case LogicOp::Invert: vklogicOp = VK_LOGIC_OP_INVERT; break;
        case LogicOp::OrReverse: vklogicOp = VK_LOGIC_OP_OR_REVERSE; break;
        case LogicOp::CopyInverted: vklogicOp = VK_LOGIC_OP_COPY_INVERTED; break;
        case LogicOp::OrInverted: vklogicOp = VK_LOGIC_OP_OR_INVERTED; break;
        case LogicOp::NAnd: vklogicOp = VK_LOGIC_OP_NAND; break;
        case LogicOp::Set: vklogicOp = VK_LOGIC_OP_SET; break;
      }
      m_ColorBlendState.logicOp = vklogicOp;
    }
  }

  PipelineColorBlendState::PipelineColorBlendState(const PipelineColorBlendAttachments& attachments, float const_1, float const_2, float const_3, float const_4)
  {
    m_ColorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

    m_ColorBlendState.logicOpEnable = VK_FALSE;

    m_ColorBlendState.attachmentCount = attachments.Get().size();
    m_ColorBlendState.pAttachments = attachments.Get().data();

    m_ColorBlendState.blendConstants[0] = const_1;
    m_ColorBlendState.blendConstants[1] = const_2;
    m_ColorBlendState.blendConstants[2] = const_3;
    m_ColorBlendState.blendConstants[3] = const_4;
  }

  void PipelineDynamicStates::AddDynamicState(VkDynamicState state)
  {
    YK_ASSERT(PipelineDynamicStates::CheckRepeat(state), "[SYSTEM/VULKAN] Do not repeat dynamic states");
    m_DynamicStates.push_back(state);
  }

  VkPipelineDynamicStateCreateInfo PipelineDynamicStates::Get() const
  {
    VkPipelineDynamicStateCreateInfo dynamicStates = {};
    dynamicStates.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStates.dynamicStateCount = static_cast<uint32_t>(m_DynamicStates.size());
    dynamicStates.pDynamicStates = !m_DynamicStates.empty() ? m_DynamicStates.data() : VK_NULL_HANDLE;

    return dynamicStates;
  }

  bool PipelineDynamicStates::CheckRepeat(const VkDynamicState& state)
  {
    for (const VkDynamicState& dState : m_DynamicStates)
      if (dState == state)
        return false;
    return true;
  }

  void PipelineLayoutStructure::AddDescriptorSetLayout(const std::shared_ptr<DescriptorSetLayout>& layout)
  {
    m_DescriptorSetLayouts.push_back(layout->Get());
  }

  void PipelineLayoutStructure::AddPushConstantRange(VkShaderStageFlags shader_stage, uint32_t offset, uint32_t size)
  {
    m_PushConstantRanges.push_back({shader_stage, offset, size});
  }

  VkPipelineLayoutCreateInfo PipelineLayoutStructure::Get() const
  {
    VkPipelineLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.setLayoutCount = static_cast<uint32_t>(m_DescriptorSetLayouts.size());
    createInfo.pSetLayouts = m_DescriptorSetLayouts.data();
    createInfo.pushConstantRangeCount = static_cast<uint32_t>(m_PushConstantRanges.size());
    createInfo.pPushConstantRanges = m_PushConstantRanges.data();

    return createInfo;
  }

  PipelineLayout::~PipelineLayout()
  {
    vkDestroyPipelineLayout(Renderer::GetDevice()->GetLogical(), m_Layout, VK_NULL_HANDLE);
  }

  std::shared_ptr<PipelineLayout> PipelineLayout::Create(const PipelineLayoutStructure& structure)
  {
    std::shared_ptr<PipelineLayout> layout = std::make_shared<PipelineLayout>();

    VkPipelineLayoutCreateInfo createInfo = structure.Get();

    VkResult result = vkCreatePipelineLayout(Renderer::GetDevice()->GetLogical(), &createInfo, VK_NULL_HANDLE, &layout->m_Layout);
    YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the pipeline layout. Error: {}", Utils::VkResultToString(result));

    return layout;
  }

  GraphicsPipeline::~GraphicsPipeline()
  {
    vkDestroyPipeline(Renderer::GetDevice()->GetLogical(), m_Pipeline, VK_NULL_HANDLE);
  }

  std::shared_ptr<GraphicsPipeline> GraphicsPipeline::Create(const PipelineShaders& shaders, const PipelineVertexInputState& input, const PipelineInputAssemblyState& assembly, const PipelineViewportScissorState& viewport_scissor, const PipelineRasterizationState& rasterization, const PipelineMultisampleState& multisample, const PipelineDepthStencilState& depth_stencil, const PipelineColorBlendState& color_blend, const PipelineDynamicStates& dynamic_states, const std::shared_ptr<PipelineLayout>& layout, const std::shared_ptr<RenderPass>& renderpass, uint32_t subpass, const PipelineCreationSpecification& spec)
  {
    std::shared_ptr<GraphicsPipeline> pipeline = std::make_shared<GraphicsPipeline>();

    VkGraphicsPipelineCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.flags = spec.Flags;
    createInfo.stageCount = static_cast<uint32_t>(shaders.Get().size());
    createInfo.pStages = shaders.Get().data();
    VkPipelineVertexInputStateCreateInfo inputState = input.Get();
    createInfo.pVertexInputState = &inputState;
    VkPipelineInputAssemblyStateCreateInfo assemplyState = assembly.Get();
    createInfo.pInputAssemblyState = &assemplyState;
    VkPipelineViewportStateCreateInfo viewportScissorState = viewport_scissor.Get();
    createInfo.pViewportState = &viewportScissorState;
    createInfo.pRasterizationState = &rasterization.Get();
    createInfo.pMultisampleState = &multisample.Get();
    createInfo.pDepthStencilState = &depth_stencil.Get();
    VkPipelineColorBlendStateCreateInfo colorBlendState = color_blend.Get();
    createInfo.pColorBlendState = &colorBlendState;
    VkPipelineDynamicStateCreateInfo dynamicStates = dynamic_states.Get();
    createInfo.pDynamicState = &dynamicStates;
    createInfo.layout = layout->Get();
    createInfo.renderPass = renderpass->Get();
    createInfo.subpass = subpass;
    createInfo.basePipelineHandle = spec.BasePipelineHandle;
    createInfo.basePipelineIndex = -1;

    VkResult result = vkCreateGraphicsPipelines(Renderer::GetDevice()->GetLogical(), Renderer::GetPipelineCache()->Get(), 1, &createInfo, VK_NULL_HANDLE, &pipeline->m_Pipeline);
    YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the pipeline. Error: {}", Utils::VkResultToString(result));

    return pipeline;
  }

  std::shared_ptr<GraphicsPipeline> GraphicsPipeline::Create(const PipelineShaders& shaders, const PipelineVertexInputState& input, const PipelineInputAssemblyState& assembly, const PipelineTessellationState& tessellation, const PipelineViewportScissorState& viewport_scissor, const PipelineRasterizationState& rasterization, const PipelineMultisampleState& multisample, const PipelineDepthStencilState& depth_stencil, const PipelineColorBlendState& color_blend, const PipelineDynamicStates& dynamic_states, const std::shared_ptr<PipelineLayout>& layout, const std::shared_ptr<RenderPass>& renderpass, uint32_t subpass, const PipelineCreationSpecification& spec)
  {
    std::shared_ptr<GraphicsPipeline> pipeline = std::shared_ptr<GraphicsPipeline>();

    VkGraphicsPipelineCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.flags = spec.Flags;
    createInfo.stageCount = static_cast<uint32_t>(shaders.Get().size());
    createInfo.pStages = shaders.Get().data();
    VkPipelineVertexInputStateCreateInfo inputState = input.Get();
    createInfo.pVertexInputState = &inputState;
    VkPipelineInputAssemblyStateCreateInfo assemplyState = assembly.Get();
    createInfo.pInputAssemblyState = &assemplyState;
    createInfo.pTessellationState = &tessellation.Get();
    VkPipelineViewportStateCreateInfo viewportScissorState = viewport_scissor.Get();
    createInfo.pViewportState = &viewportScissorState;
    createInfo.pRasterizationState = &rasterization.Get();
    createInfo.pMultisampleState = &multisample.Get();
    createInfo.pDepthStencilState = &depth_stencil.Get();
    VkPipelineColorBlendStateCreateInfo colorBlendState = color_blend.Get();
    createInfo.pColorBlendState = &colorBlendState;
    VkPipelineDynamicStateCreateInfo dynamicStates = dynamic_states.Get();
    createInfo.pDynamicState = &dynamicStates;
    createInfo.layout = layout->Get();
    createInfo.renderPass = renderpass->Get();
    createInfo.subpass = subpass;
    createInfo.basePipelineHandle = spec.BasePipelineHandle;
    createInfo.basePipelineIndex = -1;

    VkResult result = vkCreateGraphicsPipelines(Renderer::GetDevice()->GetLogical(), Renderer::GetPipelineCache()->Get(), 1, &createInfo, VK_NULL_HANDLE, &pipeline->m_Pipeline);
    YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the pipeline. Error: {}", Utils::VkResultToString(result));

    return pipeline;
  }

}