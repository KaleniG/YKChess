#include "YKLib.h"

#include "Core/WindowManager.h"
#include "Core/Input.h"
#include "Rendering/DebugOverlayManager.h"
#include "Rendering/Renderer.h"
#include "Rendering/Vertex.h"
#include "Rendering/Utils.h"

namespace yk
{
  void Renderer::Init()
  {
    YK_ASSERT(!Renderer::IsInitialized(), "The renderer has already been initialized");
    Renderer::Get().s_Initialized = true;

    Renderer::Get().s_Device = Device::Create();

    Renderer::Get().s_PipelineCache = PipelineCache::Create("YKChess.pcache");

    Renderer::Get().s_CommandPool = CommandPool::Create(Renderer::Get().s_Device->GetGraphicsQueueFamilyIndex(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    for (std::shared_ptr<CommandBuffer>& cb : Renderer::Get().s_CommandBuffers)
      cb = CommandBuffer::Allocate(Renderer::Get().s_CommandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
      Renderer::Get().s_InFlightFences[i] = Fence::Create(VK_FENCE_CREATE_SIGNALED_BIT);
      Renderer::Get().s_RenderFinishedSemaphores[i] = Semaphore::Create();
      Renderer::Get().s_ImageAvailableSemaphores[i] = Semaphore::Create();
    }
    
    VkSampleCountFlagBits sampleCount = Renderer::GetDevice()->GetMaxSampleCount();

    RenderPassStructure renderPassStructure;
    renderPassStructure.NewSubpass(sampleCount);

    // Color attachment — always added
    renderPassStructure.AddColorAttachment(
      0,
      VK_FORMAT_B8G8R8A8_SRGB,
      VK_ATTACHMENT_LOAD_OP_CLEAR,
      VK_ATTACHMENT_STORE_OP_STORE, // Store so it can be presented
      VK_IMAGE_LAYOUT_UNDEFINED,
      (sampleCount == VK_SAMPLE_COUNT_1_BIT)
      ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR   // Direct present if single-sample
      : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    );


    // Depth attachment — always added
    renderPassStructure.AddDepthStencilAttachment(
      1,
      VK_FORMAT_D32_SFLOAT,
      VK_ATTACHMENT_LOAD_OP_CLEAR,
      VK_ATTACHMENT_STORE_OP_DONT_CARE,
      VK_IMAGE_LAYOUT_UNDEFINED,
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    );

    // Resolve attachment — only if MSAA enabled
    if (sampleCount > VK_SAMPLE_COUNT_1_BIT) {
      renderPassStructure.AddResolveAttachment(
        2,
        VK_FORMAT_B8G8R8A8_SRGB,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
      );
    }

    // Subpass dependency — same in both cases
    renderPassStructure.SubpassDependency(
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
      0,
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
    );
    Renderer::Get().s_RenderPass = RenderPass::Create(renderPassStructure.GetDataAndReset());
    Renderer::Get().s_Swapchain = Swapchain::Create(Renderer::Get().s_RenderPass);


    renderPassStructure = RenderPassStructure();
    renderPassStructure.NewSubpass();

    renderPassStructure.AddColorAttachment(
      0,
      VK_FORMAT_R32_UINT,
      VK_ATTACHMENT_LOAD_OP_CLEAR,
      VK_ATTACHMENT_STORE_OP_STORE,
      VK_IMAGE_LAYOUT_UNDEFINED,
      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    );

    renderPassStructure.AddDepthStencilAttachment(
      1,
      VK_FORMAT_D32_SFLOAT,
      VK_ATTACHMENT_LOAD_OP_CLEAR,
      VK_ATTACHMENT_STORE_OP_DONT_CARE,
      VK_IMAGE_LAYOUT_UNDEFINED,
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    );

    renderPassStructure.SubpassDependency(
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
      0,
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
    );

    Renderer::Get().s_MousePickingRenderPassData = renderPassStructure.GetDataAndReset();

    Renderer::Get().s_MousePickingRenderPass = RenderPass::Create(Renderer::Get().s_MousePickingRenderPassData);

    // Mouse picking attachment & framebuffer
    glm::uvec2 windowSize = WindowManager::GetWindowSize();
    Renderer::Get().s_MousePickingColorAttachement = ColorAttachment::Create(VK_FORMAT_R32_UINT, { windowSize.x, windowSize.y }, YK_DEFAULT_VIEW_COMPONENT_MAPPING, true);
    Renderer::Get().s_MousePickingColorAttachement->Transition(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    Renderer::Get().s_MousePickingDepthStencilAttachement = DepthStencilAttachment::Create(Renderer::GetDevice()->GetDepthAttachmentFormat(), { windowSize.x, windowSize.y });

    FrameBufferStructure fStructure(Renderer::Get().s_MousePickingRenderPassData);
    fStructure.AddView(Renderer::Get().s_MousePickingColorAttachement);
    fStructure.AddView(Renderer::Get().s_MousePickingDepthStencilAttachement);

    Renderer::Get().s_MousePickingFramebuffer = FrameBuffer::Create(Renderer::Get().s_MousePickingRenderPass, fStructure);

    // Descriptor stuff
    DescriptorSetLayoutConfig shaderDescriptorSetLayout;
    shaderDescriptorSetLayout.Add(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    shaderDescriptorSetLayout.Add(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    Renderer::Get().s_ShaderDescriptorSetLayout = DescriptorSetLayout::Create(shaderDescriptorSetLayout);
    Renderer::Get().s_DescriptorPool = DescriptorPool::Create({ Renderer::Get().s_ShaderDescriptorSetLayout }, true);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
      Renderer::Get().s_PipelineDescriptorSets[i] = DescriptorSet::Allocate(Renderer::Get().s_ShaderDescriptorSetLayout, Renderer::Get().s_DescriptorPool);
    }

    Renderer::Get().s_VertShader = Shader::Create("Assets/Shaders/Shader.vert.spv");
    Renderer::Get().s_FragShader = Shader::Create("Assets/Shaders/Shader.frag.spv");
    Renderer::Get().s_MousePickingVertShader = Shader::Create("Assets/Shaders/MousePicking.vert.spv");
    Renderer::Get().s_MousePickingFragShader = Shader::Create("Assets/Shaders/MousePicking.frag.spv");

    PipelineLayoutStructure basicLayoutStructure;
    basicLayoutStructure.AddDescriptorSetLayout(Renderer::Get().s_ShaderDescriptorSetLayout);

    Renderer::Get().s_PipelineLayout = PipelineLayout::Create(basicLayoutStructure);

    {
      PipelineShaders pShaderStages;
      pShaderStages.AddShader(Renderer::Get().s_VertShader, "main");
      pShaderStages.AddShader(Renderer::Get().s_FragShader, "main");

      ////////////// MOUSE PICKING
      PipelineShaders pMousePickingShaderStages;
      pMousePickingShaderStages.AddShader(Renderer::Get().s_MousePickingVertShader, "main");
      pMousePickingShaderStages.AddShader(Renderer::Get().s_MousePickingFragShader, "main");
      ////////////

      PipelineVertexInputState pInputState;
      pInputState.AddBinding(0, sizeof(Vertex), InputRate::Vertex);
      pInputState.AddAttribute(0, VertexInputFormat::VEC3, offsetof(Vertex, Position));
      pInputState.AddAttribute(1, VertexInputFormat::VEC2, offsetof(Vertex, UV));
      pInputState.AddAttribute(2, VertexInputFormat::UINT, offsetof(Vertex, ID));

      PipelineInputAssemblyState pInputAssembly(InputPrimitiveTopology::TriangleList);

      PipelineViewportScissorState pViewportScissorState;

      PipelineRasterizationState pRasterizationState(true, false, PolygonMode::Fill, CullMode::Back, FrontFace::Clockwise);

      PipelineMultisampleState pMultisampleState(Renderer::GetDevice()->GetMaxSampleCount(), false);

      ////////////// MOUSE PICKING
      PipelineMultisampleState pMousePickingMultisampleState(VK_SAMPLE_COUNT_1_BIT, false);
      ////////////

      DepthTestInfo depthTestInfo;
      depthTestInfo.EnableWrite = false;
      depthTestInfo.CompareOp = CompareOp::Less;
      depthTestInfo.EnableBoundsTest = false;

      PipelineDepthStencilState pDepthStencilState(depthTestInfo);

      PipelineColorBlendAttachments colorBlendAttachments;
      colorBlendAttachments.AddAttachmentState
      (
        VK_BLEND_FACTOR_SRC_ALPHA,
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        VK_BLEND_OP_ADD,
        VK_BLEND_FACTOR_ONE,
        VK_BLEND_FACTOR_ZERO,
        VK_BLEND_OP_ADD,
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
      );
      PipelineColorBlendState pColorBlendingState(colorBlendAttachments);

      ////////////// MOUSE PICKING
      PipelineColorBlendAttachments mousePickingColorBlendAttachments;
      mousePickingColorBlendAttachments.AddAttachmentState
      (
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
      );
      PipelineColorBlendState pMousePickingColorBlendingState(mousePickingColorBlendAttachments);
      ////////////

      PipelineDynamicStates pDynamicStates;
      pDynamicStates.AddDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
      pDynamicStates.AddDynamicState(VK_DYNAMIC_STATE_SCISSOR);

      PipelineCreationSpecification spec;
      spec.Flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;
#if defined(CONFIG_DEBUG)
      spec.Flags |= VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;
#endif

      Renderer::Get().s_Pipeline = GraphicsPipeline::Create
      (
        pShaderStages,
        pInputState,
        pInputAssembly,
        pViewportScissorState,
        pRasterizationState,
        pMultisampleState,
        pDepthStencilState,
        pColorBlendingState,
        pDynamicStates,
        Renderer::Get().s_PipelineLayout,
        Renderer::Get().s_RenderPass,
        0,
        spec
      );

      spec.Flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
#if defined(CONFIG_DEBUG)
      spec.Flags |= VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;
#endif
      spec.BasePipelineHandle = Renderer::Get().s_Pipeline->Get();

      Renderer::Get().s_MousePickingPipeline = GraphicsPipeline::Create
      (
        pMousePickingShaderStages,
        pInputState,
        pInputAssembly,
        pViewportScissorState,
        pRasterizationState,
        pMousePickingMultisampleState,
        pDepthStencilState,
        pMousePickingColorBlendingState,
        pDynamicStates,
        Renderer::Get().s_PipelineLayout,
        Renderer::Get().s_MousePickingRenderPass,
        0,
        spec
      );
    }

    for (std::shared_ptr<MUniformBuffer>& ub : Renderer::Get().s_UniformBuffers)
      ub = MUniformBuffer::Create(sizeof(ShaderUBO));

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
      DescriptorSetUpdateData basicPipelineUpdateData = {};
      basicPipelineUpdateData.Write(0, Renderer::Get().s_UniformBuffers[i]);

      Renderer::Get().s_PipelineDescriptorSets[i]->Update(basicPipelineUpdateData);
    }
  }

  void Renderer::Shutdown()
  {
    YK_ASSERT(Renderer::IsInitialized(), "The renderer hasn't been initialized yet");
  }

  void Renderer::WaitIdle()
  {
    YK_ASSERT(Renderer::IsInitialized(), "The renderer hasn't been initialized yet");
    Renderer::Get().s_Device->WaitIdle();
  }

  void Renderer::SetImageSlot(uint32_t slot, std::shared_ptr<ImageResource> image)
  {
    Renderer::WaitIdle();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
      DescriptorSetUpdateData basicPipelineUpdateData = {};
      basicPipelineUpdateData.Write(0, Renderer::Get().s_UniformBuffers[i]);
      basicPipelineUpdateData.Write(1, image->GetTexture()->GetImage());

      Renderer::Get().s_PipelineDescriptorSets[i]->Update(basicPipelineUpdateData);
    }
  }

  void Renderer::DrawImage(glm::vec3 position, glm::vec3 size, uint32_t id, SubTexture subtexture)
  {
    YK_ASSERT(Renderer::IsInitialized(), "The renderer hasn't been initialized yet");

    Renderer::Get().s_ObjectCount++;

    std::array<glm::vec3, 4> vertices =
    {
       glm::vec3(-0.5f, -0.5f, 0.0f),
       glm::vec3(0.5f, -0.5f, 0.0f),
       glm::vec3(0.5f, 0.5f, 0.0f),
       glm::vec3(-0.5f, 0.5f, 0.0f)
    };

    for (int i = 0; i < 4; i++)
    {
      Vertex vertex;
      vertex.Position = vertices[i] * size + position;
      vertex.UV = subtexture.UVCoordinates[i];
      vertex.ID = id;
      Renderer::Get().s_VertexBatchVector.push_back(vertex);
    }

    std::array<uint32_t, 6> indices =
    {
    0, 1, 2,
    2, 3, 0
    };

    for (int i = 0; i < 6; i++)
    {
      Renderer::Get().s_IndexBatchVector.reserve(6);
      Renderer::Get().s_IndexBatchVector.emplace_back(indices[i] + (4 * (Renderer::Get().s_ObjectCount - 1)));
    }
  }

  void Renderer::EndBatch()
  {
    Renderer::WaitIdle();

    Renderer::Get().s_VertexBuffer = VertexBuffer::Create(Renderer::Get().s_VertexBatchVector);
    Renderer::Get().s_IndexBuffer = IndexBuffer::Create(Renderer::Get().s_IndexBatchVector);
  }

  void Renderer::ResetBatch()
  {
    Renderer::WaitIdle();

    Renderer::Get().s_VertexBuffer.reset();
    Renderer::Get().s_VertexBatchVector.clear();
    Renderer::Get().s_IndexBuffer.reset();
    Renderer::Get().s_IndexBatchVector.clear();
    Renderer::Get().s_ObjectCount = 0;
  }

  void Renderer::DrawText(const std::string& text, glm::vec2 size, glm::vec2 position, glm::vec4 color, std::shared_ptr<FontResource> font)
  {
    YK_ASSERT(Renderer::IsInitialized(), "The renderer hasn't been initialized yet");
  }

  uint32_t Renderer::GetPositionID(glm::uvec2 position)
  {
    YK_ASSERT(Renderer::IsInitialized(), "The renderer hasn't been initialized yet");

    StagingBuffer buffer = StagingBuffer::Create(sizeof(uint32_t));
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { static_cast<int32_t>(position.x), static_cast<int32_t>(position.y), 0 };
    region.imageExtent = { 1, 1, 1 };

    Renderer::Get().s_MousePickingColorAttachement->CopyToBuffer(buffer, region);

    buffer.Map(sizeof(uint32_t));
    uint32_t id;
    buffer.GetData(&id, sizeof(id));
    buffer.Unmap();

    Renderer::Get().s_MousePickingColorAttachement->Transition(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    return id;
  }

  void Renderer::UpdateMPFramebuffer()
  {
    YK_ASSERT(Renderer::IsInitialized(), "The renderer hasn't been initialized yet");
    Renderer::Get().s_UpdateMousePicking = true;
  }

  void Renderer::Render()
  {
    YK_ASSERT(Renderer::IsInitialized(), "The renderer hasn't been initialized yet");

    Renderer::Get().s_InFlightFences[Renderer::Get().s_CurrentFrame]->Wait();

    Renderer::Get().s_CurrentImageIndex = Renderer::Get().s_Swapchain->AcquireNextImage(Renderer::Get().s_ImageAvailableSemaphores[Renderer::Get().s_CurrentFrame]);
    if (Renderer::Get().s_CurrentImageIndex == -1)
    {
      Renderer::Get().s_CurrentFrame = (Renderer::Get().s_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
      return;
    }

    Renderer::Get().s_InFlightFences[Renderer::Get().s_CurrentFrame]->Reset();

    float aspect = static_cast<float>(WindowManager::GetWindowSize().x) / static_cast<float>(WindowManager::GetWindowSize().y);
    glm::mat4 projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);

    ShaderUBO ubo;
    ubo.Projection = projection;
    ubo.MousePosition = Input::GetMousePosition();

    Renderer::Get().s_UniformBuffers[Renderer::Get().s_CurrentFrame]->UpdateData(&ubo);

    /////////////////////////////////////
    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags = 0;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;

    VkResult result = vkBeginCommandBuffer(Renderer::Get().s_CommandBuffers[Renderer::Get().s_CurrentFrame]->Get(), &commandBufferBeginInfo);
    YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to begin recording command buffer. Error: {}", Utils::VkResultToString(result));

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.5f, 0.5f, 0.5f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = Renderer::Get().s_RenderPass->Get();
    renderPassBeginInfo.framebuffer = Renderer::Get().s_Swapchain->GetFramebuffers()[Renderer::Get().s_CurrentImageIndex];
    renderPassBeginInfo.renderArea.offset = { 0, 0 };
    renderPassBeginInfo.renderArea.extent = Renderer::Get().s_Swapchain->GetExtent();
    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(Renderer::Get().s_CommandBuffers[Renderer::Get().s_CurrentFrame]->Get(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(Renderer::Get().s_Swapchain->GetExtent().width);
    viewport.height = static_cast<float>(Renderer::Get().s_Swapchain->GetExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(Renderer::Get().s_CommandBuffers[Renderer::Get().s_CurrentFrame]->Get(), 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = Renderer::Get().s_Swapchain->GetExtent();
    vkCmdSetScissor(Renderer::Get().s_CommandBuffers[Renderer::Get().s_CurrentFrame]->Get(), 0, 1, &scissor);

    VkBuffer vertexBuffers[] = { Renderer::Get().s_VertexBuffer->Get() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(Renderer::Get().s_CommandBuffers[Renderer::Get().s_CurrentFrame]->Get(), 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(Renderer::Get().s_CommandBuffers[Renderer::Get().s_CurrentFrame]->Get(), Renderer::Get().s_IndexBuffer->Get(), 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(Renderer::Get().s_CommandBuffers[Renderer::Get().s_CurrentFrame]->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, Renderer::Get().s_PipelineLayout->Get(), 0, 1, &Renderer::Get().s_PipelineDescriptorSets[Renderer::Get().s_CurrentFrame]->Get(), 0, VK_NULL_HANDLE);
    vkCmdBindPipeline(Renderer::Get().s_CommandBuffers[Renderer::Get().s_CurrentFrame]->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, Renderer::Get().s_Pipeline->Get());

    vkCmdDrawIndexed(Renderer::Get().s_CommandBuffers[Renderer::Get().s_CurrentFrame]->Get(), static_cast<uint32_t>(Renderer::Get().s_IndexBuffer->GetIndexCount()), 1, 0, 0, 0);

    DebugOverlayManager::Render(Renderer::Get().s_CommandBuffers[Renderer::Get().s_CurrentFrame]);

    vkCmdEndRenderPass(Renderer::Get().s_CommandBuffers[Renderer::Get().s_CurrentFrame]->Get());

    if (Renderer::Get().s_UpdateMousePicking)
    {
      std::array<VkClearValue, 2> mousePickingClearValues{};
      mousePickingClearValues[0].color.uint32[0] = 0;
      mousePickingClearValues[1].depthStencil = { 1.0f, 0 };

      VkRenderPassBeginInfo mousePickingRenderPassBeginInfo{};
      mousePickingRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      mousePickingRenderPassBeginInfo.renderPass = Renderer::Get().s_MousePickingRenderPass->Get();
      mousePickingRenderPassBeginInfo.framebuffer = Renderer::Get().s_MousePickingFramebuffer->Get();
      mousePickingRenderPassBeginInfo.renderArea.offset = { 0, 0 };
      mousePickingRenderPassBeginInfo.renderArea.extent = Renderer::Get().s_Swapchain->GetExtent();
      mousePickingRenderPassBeginInfo.clearValueCount = static_cast<uint32_t>(mousePickingClearValues.size());
      mousePickingRenderPassBeginInfo.pClearValues = mousePickingClearValues.data();

      vkCmdBeginRenderPass(Renderer::Get().s_CommandBuffers[Renderer::Get().s_CurrentFrame]->Get(), &mousePickingRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

      vkCmdSetViewport(Renderer::Get().s_CommandBuffers[Renderer::Get().s_CurrentFrame]->Get(), 0, 1, &viewport);
      vkCmdSetScissor(Renderer::Get().s_CommandBuffers[Renderer::Get().s_CurrentFrame]->Get(), 0, 1, &scissor);

      vkCmdBindVertexBuffers(Renderer::Get().s_CommandBuffers[Renderer::Get().s_CurrentFrame]->Get(), 0, 1, vertexBuffers, offsets);
      vkCmdBindIndexBuffer(Renderer::Get().s_CommandBuffers[Renderer::Get().s_CurrentFrame]->Get(), Renderer::Get().s_IndexBuffer->Get(), 0, VK_INDEX_TYPE_UINT32);

      vkCmdBindDescriptorSets(Renderer::Get().s_CommandBuffers[Renderer::Get().s_CurrentFrame]->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, Renderer::Get().s_PipelineLayout->Get(), 0, 1, &Renderer::Get().s_PipelineDescriptorSets[Renderer::Get().s_CurrentFrame]->Get(), 0, VK_NULL_HANDLE);
      vkCmdBindPipeline(Renderer::Get().s_CommandBuffers[Renderer::Get().s_CurrentFrame]->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, Renderer::Get().s_MousePickingPipeline->Get());

      vkCmdDrawIndexed(Renderer::Get().s_CommandBuffers[Renderer::Get().s_CurrentFrame]->Get(), static_cast<uint32_t>(Renderer::Get().s_IndexBuffer->GetIndexCount()), 1, 0, 0, 0);

      vkCmdEndRenderPass(Renderer::Get().s_CommandBuffers[Renderer::Get().s_CurrentFrame]->Get());
      Renderer::Get().s_UpdateMousePicking = false;
    }

    result = vkEndCommandBuffer(Renderer::Get().s_CommandBuffers[Renderer::Get().s_CurrentFrame]->Get());
    YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to end recording command buffer. Error: {}", Utils::VkResultToString(result));
    /////////////////////////////////////

    Renderer::Get().s_CommandBuffers[Renderer::Get().s_CurrentFrame]->Submit
    (
      Renderer::Get().s_ImageAvailableSemaphores[Renderer::Get().s_CurrentFrame],
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      Renderer::Get().s_RenderFinishedSemaphores[Renderer::Get().s_CurrentFrame],
      Renderer::Get().s_InFlightFences[Renderer::Get().s_CurrentFrame]
    );

    Renderer::Get().s_Swapchain->Present
    (
      Renderer::Get().s_CurrentImageIndex,
      Renderer::Get().s_RenderFinishedSemaphores[Renderer::Get().s_CurrentFrame]
    );

    VkExtent3D mousePickingExtent = Renderer::Get().s_MousePickingColorAttachement->GetExtent();
    glm::uvec2 framebufferSize = WindowManager::GetFramebufferSize();
    if (mousePickingExtent.width != framebufferSize.x || mousePickingExtent.height != framebufferSize.y)
    {
      Renderer::WaitIdle();
      Renderer::Get().s_MousePickingColorAttachement = ColorAttachment::Create(VK_FORMAT_R32_UINT, { framebufferSize.x, framebufferSize.y }, YK_DEFAULT_VIEW_COMPONENT_MAPPING, true);
      Renderer::Get().s_MousePickingColorAttachement->Transition(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
      Renderer::Get().s_MousePickingDepthStencilAttachement = DepthStencilAttachment::Create(Renderer::GetDevice()->GetDepthAttachmentFormat(), { framebufferSize.x, framebufferSize.y });

      FrameBufferStructure fStructure(Renderer::Get().s_MousePickingRenderPassData);
      fStructure.AddView(Renderer::Get().s_MousePickingColorAttachement);
      fStructure.AddView(Renderer::Get().s_MousePickingDepthStencilAttachement);

      Renderer::Get().s_MousePickingFramebuffer = FrameBuffer::Create(Renderer::Get().s_MousePickingRenderPass, fStructure);
      Renderer::UpdateMPFramebuffer();
    }


    Renderer::Get().s_CurrentFrame = (Renderer::Get().s_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
  }

  bool Renderer::IsInitialized()
  {
    return Renderer::Get().s_Initialized;
  }

  std::shared_ptr<Device> Renderer::GetDevice()
  {
    YK_ASSERT(Renderer::IsInitialized(), "The renderer hasn't been initialized yet");
    return Renderer::Get().s_Device;
  }

  std::shared_ptr<PipelineCache> Renderer::GetPipelineCache()
  {
    YK_ASSERT(Renderer::IsInitialized(), "The renderer hasn't been initialized yet");
    return Renderer::Get().s_PipelineCache;
  }

  std::shared_ptr<RenderPass> Renderer::GetRenderPass()
  {
    YK_ASSERT(Renderer::IsInitialized(), "The renderer hasn't been initialized yet");
    return  Renderer::Get().s_RenderPass;
  }

  std::shared_ptr<DescriptorPool> Renderer::GetDescriptorPool()
  {
    YK_ASSERT(Renderer::IsInitialized(), "The renderer hasn't been initialized yet");
    return  Renderer::Get().s_DescriptorPool;
  }

  std::shared_ptr<Swapchain> Renderer::GetSwapchain()
  {
    YK_ASSERT(Renderer::IsInitialized(), "The renderer hasn't been initialized yet");
    return  Renderer::Get().s_Swapchain;
  }

}