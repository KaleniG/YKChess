#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

namespace yk
{

  class Fence
  {
  public:
    ~Fence();

    const VkFence& Get() const { return m_Fence; }

    void Wait(uint64_t timeout = UINT64_MAX);
    void Reset();

    static void Wait(const std::vector<std::shared_ptr<Fence>>& fences, bool wait_all, uint64_t timeout = UINT64_MAX);
    static void Reset(const std::vector<std::shared_ptr<Fence>>& fences);


    static std::shared_ptr<Fence> Create(VkFenceCreateFlags flags = 0);

  private:
    VkFence m_Fence = VK_NULL_HANDLE;
  };

}