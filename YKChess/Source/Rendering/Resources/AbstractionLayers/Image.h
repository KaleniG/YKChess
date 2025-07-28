#pragma once

#include "Rendering/Resources/AbstractionLayers/Resource.h"

namespace yk
{
  struct ImageTransitionSpecifics
  {
    VkAccessFlags AccessMask = VK_ACCESS_NONE;
    VkPipelineStageFlags PipelineStagesMask = VK_PIPELINE_STAGE_NONE;
  };

  struct ImageToImageCopySpecifics
  {
    std::vector<VkImageCopy> CopyRegions = {};
    bool GenerateMipmaps = false;
  };

  struct ImageToBufferCopySpecifics
  {
    std::vector<VkBufferImageCopy> CopyData = {};
    // RESERVED FOR FUTURE CHANGES
  };

  struct ImageCreateInfo
  {
    VkFormat Format;
    VkImageType Type;
    VkExtent3D Extent;

    uint32_t LayerCount = 1;
    uint32_t MipmapLevels = 1;
    bool Copiable = false;
    VkImageCreateFlags Flags = 0;
    VkImageTiling Tiling = VK_IMAGE_TILING_OPTIMAL;
    VkSampleCountFlagBits SampleCount = VK_SAMPLE_COUNT_1_BIT;
  };

  class Buffer;
  class Image : public Resource
  {
  public:
    virtual ~Image();

    const VkImage& Get() const { return m_Image; }
    const VkFormat& GetFormat() const { return m_Format; }
    const VkExtent3D& GetExtent() const { return m_Extent; }

    void Transition(const VkImageLayout& new_layout, const ImageTransitionSpecifics& specifics = {});
    void CopyToImage(Image& dst_image, const ImageToImageCopySpecifics& specifics = {});
    void CopyToImage(Image& dst_image, const VkImageCopy& copy_region, bool gen_mipmaps = false);
    void CopyToBuffer(Buffer& dst_buffer, const ImageToBufferCopySpecifics& specifics = {});
    void CopyToBuffer(Buffer& dst_buffer, const VkBufferImageCopy& copy_data);

  private:
    void ImageLayoutToAspectCheck(const VkImageLayout& layout);
    void AccessMaskToImageUsageCheck(const VkAccessFlags& access);
    VkAccessFlags ImageLayoutToAccessMask(const VkImageLayout& layout, const VkAccessFlags& access);
  
  protected:
    void CreateImage(const VkFormat& format, const VkImageType& type, const VkExtent3D& extent, const VkImageUsageFlags& usage, const VkMemoryPropertyFlags& memory_properties, const uint32_t& layer_count = 1, const uint32_t& mipmap_levels = 1, const VkImageCreateFlags& flags = 0, const VkImageTiling& tiling = VK_IMAGE_TILING_OPTIMAL, const VkSampleCountFlagBits& sample_count = VK_SAMPLE_COUNT_1_BIT);

  protected:
    VkImage m_Image = VK_NULL_HANDLE;

    VkFormat m_Format;
    VkImageType m_Type;
    VkExtent3D m_Extent;
    uint32_t m_LayerCount;
    VkImageTiling m_Tiling;
    uint32_t m_MipmapLevels;
    VkImageUsageFlags m_Usage;
    VkImageAspectFlags m_Aspect;
    VkImageCreateFlags m_CreateFlags;
    VkSampleCountFlagBits m_SampleCount;

    VkImageLayout m_CurrentLayout = VK_IMAGE_LAYOUT_UNDEFINED;

  private:
    friend class Buffer;
  };
}