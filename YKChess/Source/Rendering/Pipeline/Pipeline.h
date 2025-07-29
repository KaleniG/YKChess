#pragma once

#include <boost/pfr.hpp>
#include <vulkan/vulkan.h>

#include "Rendering/EngineComponents/RenderPass.h"
#include "Rendering/Pipeline/DescriptorSetLayout.h"
#include "Rendering/Pipeline/Shader.h"

#define YK_DEFAULT_COLOR_BLEND_MASK (VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT)

namespace yk
{

  enum class InputRate
  {
    Vertex,
    Instance
  };

  enum class VertexInputFormat
  {
    SINT,
    SVEC2,
    SVEC3,
    SVEC4,
    UINT,
    UVEC2,
    UVEC3,
    UVEC4,
    FLOAT,
    VEC2,
    VEC3,
    VEC4
  };

  enum class InputPrimitiveTopology
  {
    PointList,
    LineList,
    LineListWithAdjacency,
    LineStrip,
    LineStripWithAdjacency,
    TriangleList,
    TriangleListWithAdjacency,
    TriangleStrip,
    TriangleStripWithAdjacency,
    TriangleFan,
    PatchList
  };

  enum class PolygonMode
  {
    Point,
    Line,
    Fill
  };

  enum class CullMode
  {
    Front,
    Back,
    FrontAndBack
  };

  enum class FrontFace
  {
    Clockwise,
    CounterClockwise
  };

  struct DepthBiasInfo
  {
    float ConstantFactor;
    float Clamp;
    float SlopeFactor;
  };

  enum class MultisampleCount : uint32_t
  {
    _1 = 0x00000001,
    _2 = 0x00000002,
    _4 = 0x00000004,
    _8 = 0x00000008,
    _16 = 0x00000010,
    _32 = 0x00000020,
    _64 = 0x00000040
  };

  enum class CompareOp
  {
    Never,
    Always,
    Less,
    LessOrEqual,
    Greater,
    GreaterOrEqual,
    Equal,
    NotEqual
  };

  struct DepthTestInfo
  {
    bool EnableWrite;
    CompareOp CompareOp;
    bool EnableBoundsTest;
    float MinBounds = 0.0f;
    float MaxBounds = 1.0f;
  };

  struct StencilTestInfo
  {
    VkStencilOpState FrontOpState;
    VkStencilOpState BackOpState;
  };

  enum class LogicOp 
  {
    Clear,
    And,
    AndReverse,
    Copy,
    AndInverted,
    NoOp,
    XOr,
    Or,
    NOr,
    Equivalent,
    Invert,
    OrReverse,
    CopyInverted,
    OrInverted,
    NAnd,
    Set
  };

  using MultisampleMask = uint32_t;
  using Viewport = VkViewport;
  using Scissor = VkRect2D;

  class PipelineShaders
  {
  public:
    ~PipelineShaders();

    void AddShader(const std::shared_ptr<Shader>& shader, const char* entry_point);
    template<typename T> void AddSpecializationConstantBlock(const T& data);

    const std::vector<VkPipelineShaderStageCreateInfo>& Get() const { return m_ShaderStages; }

  private:
    bool CheckForShaderCount(Shader::Type type);
    VkShaderStageFlagBits StageFromShaderType(Shader::Type type);

  private:
    std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
    std::vector<VkSpecializationInfo> m_SpecializationInfos;
    std::vector<std::vector<VkSpecializationMapEntry>> m_MapEntries;
  };

  class PipelineVertexInputState
  {
  public:
    void AddBinding(uint32_t binding, uint32_t stride, InputRate input_rate); // At least 16 can be done
    void AddAttribute(uint32_t location, VertexInputFormat format, uint32_t offset);

    VkPipelineVertexInputStateCreateInfo Get() const;

  private:
    std::vector<VkVertexInputBindingDescription> m_VertexInputBindings;
    std::vector<VkVertexInputAttributeDescription> m_VertexInputAttributes;
  };

  class PipelineInputAssemblyState
  {
  public:
    PipelineInputAssemblyState(InputPrimitiveTopology topology, bool reset_primitive = false);

    const VkPipelineInputAssemblyStateCreateInfo& Get() const { return m_InputAssemblyState; }

  private:
    bool CheckResetPrimitiveToTopology(InputPrimitiveTopology topology, bool reset_primitive);

  private:
    VkPipelineInputAssemblyStateCreateInfo m_InputAssemblyState = {};
  };

  class PipelineTessellationState
  {
  public:
    PipelineTessellationState(uint32_t patch_control_points);

    const VkPipelineTessellationStateCreateInfo& Get() const { return m_TessellationState; }

  private:
    VkPipelineTessellationStateCreateInfo m_TessellationState = {};
  };

  class PipelineViewportScissorState
  {
  public:
    // can have more than only if multiViewports feature enabled
    void AddViewport(Viewport viewport);
    void AddViewport(Viewport viewport, Scissor scissor);

    VkPipelineViewportStateCreateInfo Get() const;

  private:
    std::vector<Viewport> m_Viewports;
    std::vector<Scissor> m_Scissors;
  };

  class PipelineRasterizationState
  {
  public:
    // LINES can be more than 1.0f but gpu needs to support it
    PipelineRasterizationState(bool enable_rasterization, bool depth_clamp, PolygonMode polygon_mode, CullMode cull_mode, FrontFace front_face, float line_width = 1.0f);
    PipelineRasterizationState(bool enable_rasterization, bool depth_clamp, PolygonMode polygon_mode, CullMode cull_mode, FrontFace front_face, DepthBiasInfo depth_bias, float line_width = 1.0f);

    const VkPipelineRasterizationStateCreateInfo& Get() const { return m_RasterizationState; }

  private:
    VkPipelineRasterizationStateCreateInfo m_RasterizationState = {};
  };

  class PipelineMultisampleState
  {
  public:
    //Check supported sample resolution
    // sampleRateShading for enable_shading
    PipelineMultisampleState(MultisampleCount count, bool enable_shading, float min_sample_shading = 1.0f, MultisampleMask sample_mask = 0, bool alpha_to_coverage = false, bool alpha_to_one = false);
    PipelineMultisampleState(VkSampleCountFlagBits count, bool enable_shading, float min_sample_shading = 1.0f, MultisampleMask sample_mask = 0, bool alpha_to_coverage = false, bool alpha_to_one = false);

    const VkPipelineMultisampleStateCreateInfo& Get() const { return m_MultisamplingState; }

  private:
    VkPipelineMultisampleStateCreateInfo m_MultisamplingState = {};
    MultisampleMask m_SampleMask;
  };

  class PipelineDepthStencilState
  {
  public:
    PipelineDepthStencilState();
    PipelineDepthStencilState(DepthTestInfo depth_test);
    PipelineDepthStencilState(StencilTestInfo stencil_test);
    PipelineDepthStencilState(DepthTestInfo depth_test, StencilTestInfo stencil_test);

    const VkPipelineDepthStencilStateCreateInfo& Get() const { return m_DepthStencilState; }

  private:
    VkPipelineDepthStencilStateCreateInfo m_DepthStencilState = {};
  };

  class PipelineColorBlendAttachments
  {
  public:
    PipelineColorBlendAttachments() = default;
    PipelineColorBlendAttachments(const RenderPassData& data);

    void AddAttachmentState(VkColorComponentFlags color_write_mask = YK_DEFAULT_COLOR_BLEND_MASK);
    void AddAttachmentState(VkBlendFactor src_color_blend_factor, VkBlendFactor dst_color_blend_factor, VkBlendOp color_blend_op, VkBlendFactor src_alpha_blend_factor, VkBlendFactor dst_alpha_blend_factor, VkBlendOp alpha_blend_op, VkColorComponentFlags color_write_mask = YK_DEFAULT_COLOR_BLEND_MASK);

    const std::vector<VkPipelineColorBlendAttachmentState>& Get() const { return m_States; }

  private:
    std::vector<VkPipelineColorBlendAttachmentState> m_States;
  };

  class PipelineColorBlendState
  {
  public:
    PipelineColorBlendState(LogicOp op);
    PipelineColorBlendState(const PipelineColorBlendAttachments& attachments, float const_1 = 0.0f, float const_2 = 0.0f, float const_3 = 0.0f, float const_4 = 0.0f);

    const VkPipelineColorBlendStateCreateInfo Get() const { return m_ColorBlendState; }

  private:
    VkPipelineColorBlendStateCreateInfo m_ColorBlendState = {};
  };

  class PipelineDynamicStates
  {
  public:
    void AddDynamicState(VkDynamicState state);

    VkPipelineDynamicStateCreateInfo Get() const;

  private:
    bool CheckRepeat(const VkDynamicState& state);

  private:
    std::vector<VkDynamicState> m_DynamicStates;
  };

  class PipelineLayoutStructure
  {
  public:
    void AddDescriptorSetLayout(const std::shared_ptr<DescriptorSetLayout>& layout);
    void AddPushConstantRange(VkShaderStageFlags shader_stage, uint32_t offset, uint32_t size);

    VkPipelineLayoutCreateInfo Get() const;

  private:
    std::vector<VkDescriptorSetLayout> m_DescriptorSetLayouts;
    std::vector<VkPushConstantRange> m_PushConstantRanges;
  };

  class PipelineLayout
  {
  public:
    ~PipelineLayout();

    const VkPipelineLayout& Get() const { return m_Layout; }

    static std::shared_ptr<PipelineLayout> Create(const PipelineLayoutStructure& structure);

  private:
    VkPipelineLayout m_Layout = VK_NULL_HANDLE;
  };

  struct PipelineCreationSpecification
  {
    VkPipelineCreateFlags Flags = 0;
    VkPipeline BasePipelineHandle = VK_NULL_HANDLE;
  };

  class GraphicsPipeline
  {
  public:
    ~GraphicsPipeline();

    const VkPipeline& Get() const { return m_Pipeline; }

    static std::shared_ptr<GraphicsPipeline> Create
    (
      const PipelineShaders& shaders,
      const PipelineVertexInputState& input,
      const PipelineInputAssemblyState& assembly,
      const PipelineViewportScissorState& viewport_scissor,
      const PipelineRasterizationState& rasterization,
      const PipelineMultisampleState& multisample,
      const PipelineDepthStencilState& depth_stencil,
      const PipelineColorBlendState& color_blend,
      const PipelineDynamicStates& dynamic_states,
      const std::shared_ptr<PipelineLayout>& layout,
      const std::shared_ptr<RenderPass>& renderpass,
      uint32_t subpass,
      const PipelineCreationSpecification& spec = {}
    );

    static std::shared_ptr<GraphicsPipeline> Create
    (
      const PipelineShaders& shaders,
      const PipelineVertexInputState& input,
      const PipelineInputAssemblyState& assembly,
      const PipelineTessellationState& tessellation,
      const PipelineViewportScissorState& viewport_scissor,
      const PipelineRasterizationState& rasterization,
      const PipelineMultisampleState& multisample,
      const PipelineDepthStencilState& depth_stencil,
      const PipelineColorBlendState& color_blend,
      const PipelineDynamicStates& dynamic_states,
      const std::shared_ptr<PipelineLayout>& layout,
      const std::shared_ptr<RenderPass>& renderpass,
      uint32_t subpass,
      const PipelineCreationSpecification& spec = {}
    );

  private:
    VkPipeline m_Pipeline = VK_NULL_HANDLE;
  };
  
}

#include "Rendering/Pipeline/Pipeline.inl"