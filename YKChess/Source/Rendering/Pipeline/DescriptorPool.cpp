#include <map>

#include "Rendering/Pipeline/DescriptorPool.h"
#include "Rendering/Renderer.h"
#include "Rendering/Utils.h"

#define IMGUI_FACTOR 10
#define JUST_IN_CASE_FACTOR 15

namespace yk
{

  DescriptorPool::~DescriptorPool()
  {
    vkDestroyDescriptorPool(Renderer::GetDevice()->GetLogical(), m_Pool, VK_NULL_HANDLE);
  }

  std::shared_ptr<DescriptorPool> DescriptorPool::Create(const DescriptorSetLayouts& all_layouts, bool imgui)
  {
    YK_ASSERT(!all_layouts.empty(), "[SYSTEM] Specify layouts");

    std::shared_ptr<DescriptorPool> pool = std::make_shared<DescriptorPool>();

    std::map<VkDescriptorType, uint32_t> descriptorTypesCount;

    for (const std::shared_ptr<DescriptorSetLayout>& layout : all_layouts)
    {
      for (const DescriptorSetLayoutBinding& binding : layout->GetBindings())
        descriptorTypesCount[binding.Type] += binding.Count;
    }

    if (imgui)
    {
      descriptorTypesCount[VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER] += IMGUI_FACTOR;
      descriptorTypesCount[VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER] += IMGUI_FACTOR;
    }

    std::vector<VkDescriptorPoolSize> poolSizes;
    poolSizes.reserve(descriptorTypesCount.size());
    for (const auto& type_count : descriptorTypesCount)
      poolSizes.emplace_back(type_count.first, type_count.second);

    VkDescriptorPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    createInfo.maxSets = (all_layouts.size() + (imgui ? IMGUI_FACTOR : 0)) + JUST_IN_CASE_FACTOR;
    createInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    createInfo.pPoolSizes = poolSizes.data();

    VkResult result = vkCreateDescriptorPool(Renderer::GetDevice()->GetLogical(), &createInfo, VK_NULL_HANDLE, &pool->m_Pool);
    YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the descriptor pool. Error: {}", Utils::VkResultToString(result));

    return pool;
  }

}