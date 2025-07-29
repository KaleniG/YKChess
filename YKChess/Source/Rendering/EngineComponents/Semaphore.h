#pragma once

#include <memory>

#include <vulkan/vulkan.h>

namespace yk
{

  class Semaphore
  {
  public:
    ~Semaphore();

    const VkSemaphore& Get() const { return m_Semaphore; }

    static std::shared_ptr<Semaphore> Create(VkSemaphoreCreateFlags flags = 0);

  private:
    VkSemaphore m_Semaphore = VK_NULL_HANDLE;
  };

}