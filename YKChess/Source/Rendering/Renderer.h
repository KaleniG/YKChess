#pragma once

#include <array>
#include <memory>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include "Rendering/EngineComponents/CommandBuffer.h"
#include "Rendering/EngineComponents/CommandPool.h"
#include "Rendering/EngineComponents/RenderPass.h"
#include "Rendering/EngineComponents/Semaphore.h"
#include "Rendering/EngineComponents/Fence.h"
#include "Rendering/Pipeline/DescriptorPool.h"
#include "Rendering/Pipeline/DescriptorSet.h"
#include "Rendering/Pipeline/PipelineCache.h"
#include "Rendering/Pipeline/Pipeline.h"
#include "Rendering/Pipeline/Shader.h"
#include "Rendering/Resources/MUniformBuffer.h"
#include "Rendering/Resources/VertexBuffer.h"
#include "Rendering/Resources/IndexBuffer.h"
#include "Rendering/Resources/Texture.h"
#include "Rendering/DebugOverlayManager.h"
#include "Rendering/ResourceManager.h"
#include "Rendering/ImageResource.h"
#include "Rendering/Swapchain.h"
#include "Rendering/Device.h"

#define MAX_FRAMES_IN_FLIGHT 3

namespace yk
{
  class ImageResource;
  class FontResource;

  class Renderer
  {
  public:
    static void Init();
    static void Shutdown();

    static void WaitIdle();

    static void SetImageSlot(uint32_t slot, std::shared_ptr<ImageResource> image);
    static void DrawImage(glm::vec2 position, glm::vec2 size, uint32_t slot, SubTexture subtexture);
    static void EndBatch();
    static void ResetBatch();
    static void DrawText(const std::string& text, glm::vec2 size, glm::vec2 position, glm::vec4 color, std::shared_ptr<FontResource> font);

    static void Render();

    static bool IsInitialized();

    static std::shared_ptr<Device> GetDevice();
    static std::shared_ptr<PipelineCache> GetPipelineCache();
    static std::shared_ptr<RenderPass> GetRenderPass();
    static std::shared_ptr<DescriptorPool> GetDescriptorPool();
    static std::shared_ptr<Swapchain> GetSwapchain();

  private:
    static Renderer& Get() { static Renderer instance; return instance; }
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

  private:
    bool s_Initialized = false;

    std::shared_ptr<Device> s_Device;
    std::shared_ptr<Swapchain> s_Swapchain;
    std::shared_ptr<RenderPass> s_RenderPass;
    std::shared_ptr<PipelineCache> s_PipelineCache;

    std::shared_ptr<CommandPool> s_CommandPool;
    std::array<std::shared_ptr<CommandBuffer>, MAX_FRAMES_IN_FLIGHT> s_CommandBuffers;
    std::array<std::shared_ptr<Fence>, MAX_FRAMES_IN_FLIGHT> s_InFlightFences;
    std::array<std::shared_ptr<Semaphore>, MAX_FRAMES_IN_FLIGHT> s_RenderFinishedSemaphores;
    std::array<std::shared_ptr<Semaphore>, MAX_FRAMES_IN_FLIGHT> s_ImageAvailableSemaphores;

    uint32_t s_CurrentFrame = 0;
    uint32_t s_CurrentImageIndex = 0;
    std::shared_ptr<VertexBuffer> s_VertexBuffer;
    std::shared_ptr<IndexBuffer> s_IndexBuffer;

    std::shared_ptr<DescriptorPool> s_DescriptorPool;
    std::array<std::shared_ptr<MUniformBuffer>, MAX_FRAMES_IN_FLIGHT> s_UniformBuffers;

    std::array<std::shared_ptr<DescriptorSet>, MAX_FRAMES_IN_FLIGHT> s_PipelineDescriptorSets;
    std::shared_ptr<DescriptorSetLayout> s_DescriptorSetLayout;
    std::shared_ptr<Shader> s_VertShader;
    std::shared_ptr<Shader> s_FragShader;
    std::shared_ptr<GraphicsPipeline> s_Pipeline;
    std::shared_ptr<PipelineLayout> s_PipelineLayout;

    std::vector<float> s_VertexBatchVector;
    std::vector<uint32_t> s_IndexBatchVector;
    uint32_t s_ObjectCount = 0;


  private:
    friend class DebugOverlayManager;
    friend class ResourceManager;
  };
}