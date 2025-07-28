#pragma once

#include "Rendering/Resources/AbstractionLayers/ViewImage.h"

namespace yk
{

  struct StorageImageCreateInfo
  {
    VkFormat Format;
    VkImageType Type;
    VkExtent3D Extent;
    VkComponentMapping ComponentMapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
    uint32_t LayerCount = 1;
    uint32_t MipmapLevels = 1;
    bool Copiable = false;
    VkImageCreateFlags Flags = 0;
    VkSampleCountFlagBits SampleCount = VK_SAMPLE_COUNT_1_BIT;
  };

  class StorageImage : public ViewImage
  {
  public:
    static StorageImage Create(const StorageImageCreateInfo& info);
    static StorageImage Create
    (
      const VkFormat& format, 
      const VkImageType& type, 
      const VkExtent3D& extent, 
      const VkComponentMapping& view_component_mapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY }, 
      const uint32_t& layer_count = 1, 
      const uint32_t& mipmap_levels = 1, 
      bool copiable = false, 
      const VkImageCreateFlags& flags = 0, 
      const VkSampleCountFlagBits& sample_count = VK_SAMPLE_COUNT_1_BIT
    );
  };

}