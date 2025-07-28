#pragma once

#include <vulkan/vulkan.h>

namespace yk
{
  class Resource
  {
  public:
    virtual ~Resource();

    const VkDeviceSize& GetSize() const { return m_Size; }
    const VkDeviceMemory& GetMemory() const { return m_Memory; }

  protected:
    VkPipelineStageFlags AccessMaskToPipelineStages(const VkAccessFlags& access, const VkPipelineStageFlags& stages);

  protected:
    VkDeviceSize m_Size = 0;
    VkDeviceMemory m_Memory = VK_NULL_HANDLE;

    VkAccessFlags m_CurrentAccessMask = VK_ACCESS_NONE;
    VkPipelineStageFlags m_CurrentPipelineStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
  };
}