#pragma once

#include "Rendering/Resources/AbstractionLayers/Image.h"

namespace yk
{

  struct TransferImageCreateInfo
  {
    VkFormat Format;
    VkImageType Type;
    VkExtent3D Extent;

    uint32_t LayerCount = 1;
    uint32_t MipmapLevels = 1;
    VkImageCreateFlags Flags = 0;
    VkSampleCountFlagBits SampleCount = VK_SAMPLE_COUNT_1_BIT;
  };

  class TransferImage : public Image
  {
  public:
    ~TransferImage() = default;

    static TransferImage Create(Image& src_image, bool copy = true);
    static TransferImage Create(const TransferImageCreateInfo& info);
    static TransferImage Create
    (
      const VkFormat& format, 
      const VkImageType& type, 
      const VkExtent3D& extent, 
      const uint32_t& layer_count = 1,
      const uint32_t& mipmap_levels = 1, 
      const VkImageCreateFlags& flags = 0, 
      const VkSampleCountFlagBits& sample_count = VK_SAMPLE_COUNT_1_BIT
    );
  };

}