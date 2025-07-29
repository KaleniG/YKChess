#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

#include "Rendering/Pipeline/DescriptorSetLayout.h"
#include "Rendering/Pipeline/DescriptorPool.h"
#include "Rendering/Resources/MStorageTexelBuffer.h"
#include "Rendering/Resources/StorageTexelBuffer.h"
#include "Rendering/Resources/UniformTexelBuffer.h"
#include "Rendering/Resources/MStorageBuffer.h"
#include "Rendering/Resources/MUniformBuffer.h"
#include "Rendering/Resources/StorageBuffer.h"
#include "Rendering/Resources/UniformBuffer.h"
#include "Rendering/Resources/SampledImage.h"
#include "Rendering/Resources/StorageImage.h"

namespace yk
{

  class DescriptorSetUpdateData
  {
  public:
    void Write(uint32_t binding, const std::shared_ptr<MStorageTexelBuffer>& buffer, uint32_t array_index = 0);
    void Write(uint32_t binding, const std::shared_ptr<StorageTexelBuffer>& buffer, uint32_t array_index = 0);
    void Write(uint32_t binding, const std::shared_ptr<UniformTexelBuffer>& buffer, uint32_t array_index = 0);
    void Write(uint32_t binding, const std::shared_ptr<MStorageBuffer>& buffer, uint32_t array_index = 0);
    void Write(uint32_t binding, const std::shared_ptr<StorageBuffer>& buffer, uint32_t array_index = 0);
    void Write(uint32_t binding, const std::shared_ptr<MUniformBuffer>& buffer, uint32_t array_index = 0);
    void Write(uint32_t binding, const std::shared_ptr<UniformBuffer>& buffer, uint32_t array_index = 0);
    void Write(uint32_t binding, const std::shared_ptr<StorageImage>& image, uint32_t array_index = 0);
    void Write(uint32_t binding, const std::shared_ptr<SampledImage>& image, uint32_t array_index = 0);

    // TODO:  DYNAMIC STORAGE NAD UNIFORM BUFFERS
    //        INPUT ATTACHMENT
    //        INLINE UNIFORM BLOCK

    void Write(uint32_t binding, const std::vector<std::shared_ptr<MStorageTexelBuffer>>& buffers, uint32_t count, uint32_t array_index = 0);
    void Write(uint32_t binding, const std::vector<std::shared_ptr<StorageTexelBuffer>>& buffers, uint32_t count, uint32_t array_index = 0);
    void Write(uint32_t binding, const std::vector<std::shared_ptr<UniformTexelBuffer>>& buffers, uint32_t count, uint32_t array_index = 0);
    void Write(uint32_t binding, const std::vector<std::shared_ptr<MStorageBuffer>>& buffers, uint32_t count, uint32_t array_index = 0);
    void Write(uint32_t binding, const std::vector<std::shared_ptr<StorageBuffer>>& buffers, uint32_t count, uint32_t array_index = 0);
    void Write(uint32_t binding, const std::vector<std::shared_ptr<MUniformBuffer>>& buffers, uint32_t count, uint32_t array_index = 0);
    void Write(uint32_t binding, const std::vector<std::shared_ptr<UniformBuffer>>& buffers, uint32_t count, uint32_t array_index = 0);
    void Write(uint32_t binding, const std::vector<std::shared_ptr<StorageImage>>& images, uint32_t count, uint32_t array_index = 0);
    void Write(uint32_t binding, const std::vector<std::shared_ptr<SampledImage>>& images, uint32_t count, uint32_t array_index = 0);

    void Reset();

    const std::vector<VkWriteDescriptorSet>& GetWrites() const { return m_WriteData; }

  private:
    std::vector<VkWriteDescriptorSet> m_WriteData;
    std::vector<std::vector<VkDescriptorBufferInfo>> m_DescriptorBufferInfos;
    std::vector<std::vector<VkDescriptorImageInfo>> m_DescriptorImageInfos;
    std::vector<std::vector<VkBufferView>> m_DescriptorTexelBufferViews;
  };

  struct DescriptorSetCopyInfo
  {
    uint32_t SrcArrayIndex = 0;
    uint32_t DstArrayIndex = 0;
    uint32_t ElementCount = 1;
  };

  class DescriptorSet
  {
  public:
    ~DescriptorSet();

    const VkDescriptorSet& Get() const { return m_DescriptorSet; }

    void Update(const DescriptorSetUpdateData& data);

    static void Copy(const DescriptorSet& src, uint32_t src_binding, const DescriptorSet& dst, uint32_t dst_binding, const DescriptorSetCopyInfo& info = {});

    static std::shared_ptr<DescriptorSet> Allocate(const std::shared_ptr<DescriptorSetLayout>& layout, const std::shared_ptr<DescriptorPool>& pool);

    const DescriptorSetLayoutBinding& operator[](uint32_t binding) const;

  private:
    bool CheckWriteData(const VkWriteDescriptorSet& data);
    static bool CheckCopyData(const DescriptorSet& src, uint32_t src_binding, const DescriptorSet& dst, uint32_t dst_binding, const DescriptorSetCopyInfo& info);

  private:
    VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;
    std::shared_ptr<DescriptorPool> m_Pool;
    std::shared_ptr<DescriptorSetLayout> m_Layout;
  };

}