#include "Rendering/EngineComponents/Fence.h"
#include "Rendering/Renderer.h"
#include "Rendering/Utils.h"

namespace yk
{

  Fence::~Fence()
  {
    vkDestroyFence(Renderer::GetDevice()->GetLogical(), m_Fence, VK_NULL_HANDLE);
  }

  void Fence::Wait(uint64_t timeout)
  {
    vkWaitForFences(Renderer::GetDevice()->GetLogical(), 1, &m_Fence, VK_TRUE, timeout);
  }

  void Fence::Reset()
  {
    vkResetFences(Renderer::GetDevice()->GetLogical(), 1, &m_Fence);
  }

  void Fence::Wait(const std::vector<std::shared_ptr<Fence>>& fences, bool wait_all, uint64_t timeout)
  {
    std::vector<VkFence> vkFences(fences.size());
    for (const std::shared_ptr<Fence>& fence : fences)
      vkFences.emplace_back(fence->Get());

    vkWaitForFences(Renderer::GetDevice()->GetLogical(), vkFences.size(), vkFences.data(), wait_all ? VK_TRUE : VK_FALSE, timeout);
  }

  void Fence::Reset(const std::vector<std::shared_ptr<Fence>>& fences)
  {
    std::vector<VkFence> vkFences(fences.size());
    for (const std::shared_ptr<Fence>& fence : fences)
      vkFences.emplace_back(fence->Get());

    vkResetFences(Renderer::GetDevice()->GetLogical(), vkFences.size(), vkFences.data());
  }

  std::shared_ptr<Fence> Fence::Create(VkFenceCreateFlags flags)
  {
    std::shared_ptr<Fence> fence = std::make_shared<Fence>();

    VkFenceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkResult result = vkCreateFence(Renderer::GetDevice()->GetLogical(), &createInfo, VK_NULL_HANDLE, &fence->m_Fence);
    YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create a fence. Error: {}", Utils::VkResultToString(result));

    return fence;
  }
}