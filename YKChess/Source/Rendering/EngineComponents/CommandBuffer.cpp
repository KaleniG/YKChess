#include "Rendering/EngineComponents/CommandBuffer.h"
#include "Rendering/Renderer.h"
#include "Rendering/Utils.h"

namespace yk
{

  CommandBuffer::~CommandBuffer()
  {
    if (CommandBuffer::Get())
      CommandBuffer::Deallocate();
  }

  const VkCommandBuffer& CommandBuffer::Get() const
  {
    YK_ASSERT(m_Buffer != VK_NULL_HANDLE, "[SYSTEM] The command buffer is not allocated");
    return m_Buffer;
  }

  const std::shared_ptr<CommandPool>& CommandBuffer::GetPool() const
  {
    YK_ASSERT(r_Pool != nullptr, "[SYSTEM] The command pool is not present, the command buffer is not allocated");
    return r_Pool;
  }

  void CommandBuffer::Deallocate()
  {
    vkFreeCommandBuffers(Renderer::GetDevice()->GetLogical(), CommandBuffer::GetPool()->Get(), 1, &CommandBuffer::Get());
    m_Buffer = VK_NULL_HANDLE;
  }

  void CommandBuffer::Submit(const std::shared_ptr<Semaphore>& wait_semaphore, VkPipelineStageFlags wait_stage, const std::shared_ptr<Semaphore>& signal_semaphore, const std::shared_ptr<Fence>& signal_fence)
  {
    VkSemaphore waitSemaphores[]      = { wait_semaphore->Get() };
    VkPipelineStageFlags waitStages[] = { wait_stage };
    VkSemaphore signalSemaphores[]    = { signal_semaphore->Get() };

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_Buffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VkResult result = vkQueueSubmit(Renderer::GetDevice()->GetGraphicsQueue(), 1, &submitInfo, signal_fence->Get());
    YK_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "[VULKAN] Failed to submit the draw command buffer. {}", Utils::VkResultToString(result));
  }

  std::shared_ptr<CommandBuffer> CommandBuffer::Allocate(std::shared_ptr<CommandPool> pool, VkCommandBufferLevel level)
  {
    std::shared_ptr<CommandBuffer> buffer = std::make_shared<CommandBuffer>();

    buffer->r_Pool = pool;

    VkCommandBufferAllocateInfo commandBufferAllocInfo{};
    commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocInfo.commandPool = pool->Get();
    commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocInfo.commandBufferCount = 1;

    VkResult result = vkAllocateCommandBuffers(Renderer::GetDevice()->GetLogical(), &commandBufferAllocInfo, &buffer->m_Buffer);
    YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to allocate the command buffer. {}", Utils::VkResultToString(result));

    return buffer;
  }

}