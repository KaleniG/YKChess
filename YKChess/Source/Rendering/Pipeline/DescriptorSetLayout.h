#pragma once

#include <vector>
#include <memory>
#include <set>

#include <vulkan/vulkan.h>

namespace yk
{

  struct DescriptorSetLayoutBinding
  {
    uint32_t Binding;
    VkDescriptorType Type;
    VkShaderStageFlags ShaderStage;
    uint32_t Count;
    const VkSampler* ImmutableSamplers;
  };

  using DescriptorSetLayoutBindings = std::vector<DescriptorSetLayoutBinding>;

  class DescriptorSetLayoutConfig
  {
  public:
    void Add(uint32_t binding, const VkDescriptorType& type, const VkShaderStageFlags& stage, uint32_t count = 1, const VkSampler* immutable_samplers = VK_NULL_HANDLE);

    const DescriptorSetLayoutBindings& GetBindings() const;

  private:
    std::set<uint32_t> m_UsedBindings;
    DescriptorSetLayoutBindings m_Bindings;
  };

  class DescriptorSetLayout
  {
  public:
    ~DescriptorSetLayout();

    const VkDescriptorSetLayout& Get() const { return m_Layout; }
    const DescriptorSetLayoutBindings& GetBindings() const { return m_Bindings; }

    static std::shared_ptr<DescriptorSetLayout> Create(const DescriptorSetLayoutConfig& config);

  private:
    VkDescriptorSetLayout m_Layout = VK_NULL_HANDLE;
    DescriptorSetLayoutBindings m_Bindings;
  };


}