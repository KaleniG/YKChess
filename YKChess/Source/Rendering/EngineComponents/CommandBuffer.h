#pragma once

#include <memory>

#include <vulkan/vulkan.h>

#include "Rendering/EngineComponents/CommandPool.h"
#include "Rendering/EngineComponents/Semaphore.h"
#include "Rendering/EngineComponents/Fence.h"

namespace yk
{

  class CommandBuffer
  {
  public:
    ~CommandBuffer();

    const VkCommandBuffer& Get() const;
    const std::shared_ptr<CommandPool>& GetPool() const;

    void Deallocate();

    void Submit(const std::shared_ptr<Semaphore>& wait_semaphore, VkPipelineStageFlags wait_stage, const std::shared_ptr<Semaphore>& signal_semaphore, const std::shared_ptr<Fence>& signal_fence);

    static std::shared_ptr<CommandBuffer> Allocate(std::shared_ptr<CommandPool> pool, VkCommandBufferLevel level);

  private:
    std::shared_ptr<CommandPool> r_Pool = nullptr;
    VkCommandBuffer m_Buffer = VK_NULL_HANDLE;
  };

}