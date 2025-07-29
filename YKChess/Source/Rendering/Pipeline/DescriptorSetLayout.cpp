#include "Rendering/Pipeline/DescriptorSetLayout.h"
#include "Rendering/Renderer.h"
#include "Rendering/Utils.h"

namespace yk
{

  void DescriptorSetLayoutConfig::Add(uint32_t binding, const VkDescriptorType& type, const VkShaderStageFlags& stage, uint32_t count, const VkSampler* immutable_samplers)
  {
    YK_ASSERT(!m_UsedBindings.contains(binding), "[SYSTEM] This binding is not available");

    m_UsedBindings.insert(binding);
    m_Bindings.push_back({binding, type, stage, count, immutable_samplers});
  }

  const DescriptorSetLayoutBindings& DescriptorSetLayoutConfig::GetBindings() const
  {
    YK_ASSERT(!m_Bindings.empty(), "[SYSTEM] Cannot get an empty layout");
    return m_Bindings;
  }

  DescriptorSetLayout::~DescriptorSetLayout()
  {
    vkDestroyDescriptorSetLayout(Renderer::GetDevice()->GetLogical(), m_Layout, VK_NULL_HANDLE);
  }

  std::shared_ptr<DescriptorSetLayout> DescriptorSetLayout::Create(const DescriptorSetLayoutConfig& config)
  {
    std::shared_ptr<DescriptorSetLayout> layout = std::make_shared<DescriptorSetLayout>();
    layout->m_Bindings = config.GetBindings();

    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings(config.GetBindings().size());
    for (size_t i = 0; i < config.GetBindings().size(); i++)
    {
      descriptorSetLayoutBindings[i].binding = config.GetBindings()[i].Binding;
      descriptorSetLayoutBindings[i].descriptorCount = config.GetBindings()[i].Count;
      descriptorSetLayoutBindings[i].descriptorType = config.GetBindings()[i].Type;
      descriptorSetLayoutBindings[i].pImmutableSamplers = config.GetBindings()[i].ImmutableSamplers;
      descriptorSetLayoutBindings[i].stageFlags = config.GetBindings()[i].ShaderStage;
    }

    VkDescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size());
    createInfo.pBindings = descriptorSetLayoutBindings.data();

    VkResult result = vkCreateDescriptorSetLayout(Renderer::GetDevice()->GetLogical(), &createInfo, VK_NULL_HANDLE, &layout->m_Layout);
    YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the shaders' descriptor set layout. Error: {}", Utils::VkResultToString(result));

    return layout;
  }

}