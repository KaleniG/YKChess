#pragma once

#include <vector>

#include "Rendering/Resources/AbstractionLayers/Resource.h"

namespace yk
{
  struct BufferTransitionSpecifics
  {
    VkPipelineStageFlags PipelineStagesMask = VK_PIPELINE_STAGE_NONE;
    // RESERVED FOR FUTURE CHANGES
  };

  struct BufferToBufferCopySpecifics
  {
    std::vector<VkBufferCopy> CopyRegions = {};
    // RESERVED FOR FUTURE CHANGES
  };

  struct BufferToImageCopySpecifics
  {
    std::vector<VkBufferImageCopy> CopyData = {};
    bool GenerateMipmaps = false;
  };

  struct BufferCreateInfo
  {
    VkDeviceSize Size;
    bool Copyable = false;
    bool CopyDestination = false;
  };

  struct BufferViewCreateInfo
  {
    VkFormat Format = VK_FORMAT_UNDEFINED;
    VkDeviceSize Range = 0;
    VkDeviceSize Offset = 0;
  };

  class Image;
  class Buffer : public Resource
  {
  public:
    ~Buffer();

    const VkBuffer& Get() const { return m_Buffer; }

    void Transition(const VkAccessFlags& new_access, const BufferTransitionSpecifics& specifics = {});
    void CopyToBuffer(Buffer& dst_buffer, const BufferToBufferCopySpecifics& specifics = {});
    void CopyToBuffer(Buffer& dst_buffer, const VkBufferCopy& copy_region);
    void CopyToImage(Image& dst_image, const BufferToImageCopySpecifics& specifics = {});
    void CopyToImage(Image& dst_image, const VkBufferImageCopy& copy_data, bool gen_mipmaps = false);

  private:
    void AccessMaskToBufferUsageCheck(const VkAccessFlags& access);

  protected:
    void CreateBuffer(const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& memory_properties, const VkDeviceSize& size);

  protected:
    VkBuffer m_Buffer;

    VkBufferUsageFlags m_Usage;
    VkMemoryPropertyFlags m_MemoryProperties;

  private:
    bool m_Used = false;

  private:
    friend class Image;
  };
}