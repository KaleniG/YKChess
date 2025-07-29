#include "Rendering/EngineComponents/CommandPool.h"
#include "Rendering/Renderer.h"
#include "Rendering/Utils.h"

namespace yk
{
  CommandPool::~CommandPool()
  {
    vkDestroyCommandPool(Renderer::GetDevice()->GetLogical(), m_Pool, VK_NULL_HANDLE);
  }

  std::shared_ptr<CommandPool> CommandPool::Create(uint32_t queue_family_index, VkCommandPoolCreateFlags flags)
  {
    std::shared_ptr<CommandPool> pool = std::make_shared<CommandPool>();

    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = flags;
    createInfo.queueFamilyIndex = queue_family_index;

    VkResult result = vkCreateCommandPool(Renderer::GetDevice()->GetLogical(), &createInfo, VK_NULL_HANDLE, &pool->m_Pool);
    YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the command pool. {}", Utils::VkResultToString(result));

    return pool;
  }

}