#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

#include "Rendering/Pipeline/DescriptorSetLayout.h"

namespace yk
{
  using DescriptorSetLayouts = std::vector<std::shared_ptr<DescriptorSetLayout>>;

  class DescriptorPool
  {
  public:
    ~DescriptorPool();

    const VkDescriptorPool& Get() const { return m_Pool; }

    static std::shared_ptr<DescriptorPool> Create(const DescriptorSetLayouts& all_layouts, bool imgui = true);

  private:
    VkDescriptorPool m_Pool = VK_NULL_HANDLE;
  };

}