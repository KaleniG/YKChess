#include "Rendering/EngineComponents/Semaphore.h"
#include "Rendering/Renderer.h"
#include "Rendering/Utils.h"

namespace yk
{
  Semaphore::~Semaphore()
  {
    vkDestroySemaphore(Renderer::GetDevice()->GetLogical(), m_Semaphore, VK_NULL_HANDLE);
  }

  std::shared_ptr<Semaphore> Semaphore::Create(VkSemaphoreCreateFlags flags)
  {
    std::shared_ptr<Semaphore> semaphore = std::make_shared<Semaphore>();

    VkSemaphoreCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    createInfo.flags = flags;

    VkResult result = vkCreateSemaphore(Renderer::GetDevice()->GetLogical(), &createInfo, VK_NULL_HANDLE, &semaphore->m_Semaphore);
    YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create a fence. Error: {}", Utils::VkResultToString(result));

    return semaphore;
  }

}