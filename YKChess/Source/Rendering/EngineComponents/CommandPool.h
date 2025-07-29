#pragma once

#include <memory>

#include <vulkan/vulkan.h>

namespace yk
{

  class CommandPool
  {
  public:
    ~CommandPool();

    const VkCommandPool& Get() const { return m_Pool; }

    static std::shared_ptr<CommandPool> Create(uint32_t queue_family_index, VkCommandPoolCreateFlags flags);

  private:
    VkCommandPool m_Pool = VK_NULL_HANDLE;
  };

}