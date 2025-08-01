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

    RenderPassStructure renderPassStructure;
    renderPassStructure.NewSubpass();
    renderPassStructure.AddColorAttachment(0, VK_FORMAT_B8G8R8A8_SRGB, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    renderPassStructure.AddDepthStencilAttachment(1, VK_FORMAT_D32_SFLOAT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    renderPassStructure.SubpassDependency
    (
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
      0,
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
    );

    Renderer::Get().s_RenderPass = RenderPass::Create(renderPassStructure.GetDataAndReset());
    Renderer::Get().s_Swapchain = Swapchain::Create(Renderer::Get().s_RenderPass);

    DescriptorSetLayoutConfig basicSetLayout;
    basicSetLayout.Add(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    basicSetLayout.Add(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    Renderer::Get().s_DescriptorSetLayout = DescriptorSetLayout::Create(basicSetLayout);
    Renderer::Get().s_DescriptorPool = DescriptorPool::Create({ Renderer::Get().s_DescriptorSetLayout }, true);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
      Renderer::Get().s_PipelineDescriptorSets[i] = DescriptorSet::Allocate(Renderer::Get().s_DescriptorSetLayout, Renderer::Get().s_DescriptorPool);
    }

    Renderer::Get().s_VertShader = Shader::Create("Assets/Shaders/Shader.vert.spv");
    Renderer::Get().s_FragShader = Shader::Create("Assets/Shaders/Shader.frag.spv");

    PipelineLayoutStructure basicLayoutStructure;
    basicLayoutStructure.AddDescriptorSetLayout(Renderer::Get().s_DescriptorSetLayout);

    Renderer::Get().s_PipelineLayout = PipelineLayout::Create(basicLayoutStructure);

    {
      PipelineShaders pShaderStages;
      pShaderStages.AddShader(Renderer::Get().s_VertShader, "main");
      pShaderStages.AddShader(Renderer::Get().s_FragShader, "main");

      PipelineVertexInputState pInputState;
      pInputState.AddBinding(0, sizeof(Vertex), InputRate::Vertex);
      pInputState.AddAttribute(0, VertexInputFormat::VEC2, offsetof(Vertex, Position));
      //pInputState.AddAttribute(1, VertexInputFormat::VEC3, offsetof(Vertex, Color));
      pInputState.AddAttribute(1, VertexInputFormat::VEC2, offsetof(Vertex, TextureCoord));

      PipelineInputAssemblyState pInputAssembly(InputPrimitiveTopology::TriangleList);

      PipelineViewportScissorState pViewportScissorState;

      PipelineRasterizationState pRasterizationState(true, false, PolygonMode::Fill, CullMode::Back, FrontFace::Clockwise);

      PipelineMultisampleState pMultisampleState(MultisampleCount::_1, false);

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

      PipelineDynamicStates pDynamicStates;
      pDynamicStates.AddDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
      pDynamicStates.AddDynamicState(VK_DYNAMIC_STATE_SCISSOR);

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
        0
      );
    }

    for (std::shared_ptr<MUniformBuffer>& ub : Renderer::Get().s_UniformBuffers)
      ub = MUniformBuffer::Create(sizeof(UniformBufferObject));

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

  void Renderer::DrawImage(glm::vec2 position, glm::vec2 size, uint32_t slot, SubTexture subtexture)
  {
    YK_ASSERT(Renderer::IsInitialized(), "The renderer hasn't been initialized yet");

    Renderer::Get().s_ObjectCount++;

    std::array<glm::vec2, 4> vertices =
    {
       glm::vec2(-0.5f, -0.5f),
       glm::vec2(0.5f, -0.5f),
       glm::vec2(0.5f, 0.5f),
       glm::vec2(-0.5f, 0.5f)
    };

    for (int i = 0; i < 4; i++)
    {
      vertices[i] = vertices[i] * size + position;
      Renderer::Get().s_VertexBatchVector.reserve(4);
      Renderer::Get().s_VertexBatchVector.emplace_back(vertices[i].x);
      Renderer::Get().s_VertexBatchVector.emplace_back(vertices[i].y);
      Renderer::Get().s_VertexBatchVector.emplace_back(subtexture.UVCoordinates[i].x);
      Renderer::Get().s_VertexBatchVector.emplace_back(subtexture.UVCoordinates[i].y);
    }

    std::array<uint32_t, 6> indices =
    {
    0, 1, 2,
    2, 3, 0
    };

    for (int i = 0; i < 6; i++)
    {
      Renderer::Get().s_VertexBatchVector.reserve(6);
      Renderer::Get().s_IndexBatchVector.emplace_back(indices[i] + (4 * (Renderer::Get().s_ObjectCount - 1)));
    }
  }

  void Renderer::EndBatch()
  {
    YK_ASSERT(Renderer::IsInitialized(), "The renderer hasn't been initialized yet");
    Renderer::Get().s_VertexBuffer = VertexBuffer::Create(Renderer::Get().s_VertexBatchVector);
    Renderer::Get().s_IndexBuffer = IndexBuffer::Create(Renderer::Get().s_IndexBatchVector);
  }

  void Renderer::ResetBatch()
  {
    YK_ASSERT(Renderer::IsInitialized(), "The renderer hasn't been initialized yet");

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

    UniformBufferObject ubo;
    ubo.Projection = projection;
    ubo.MousePosition = Input::GetMousePosition();

    Renderer::Get().s_UniformBuffers[Renderer::Get().s_CurrentFrame]->Update(&ubo);

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