#pragma once

#include "Rendering/Resources/Attachment.h"

namespace yk
{
  struct ColorAttachmentCreateInfo
  {
    VkFormat Format;
    VkExtent2D Extent;
    VkComponentMapping ComponentMapping = DEFAULT_VIEW_COMPONENT_MAPPING;
    bool Copiable = false;
    VkSampleCountFlagBits SampleCount = VK_SAMPLE_COUNT_1_BIT;
  };

  class ColorAttachment : public Attachment
  {
  public:
    static std::shared_ptr<ColorAttachment> Create(const ColorAttachmentCreateInfo& info);
    static std::shared_ptr<ColorAttachment> Create
    (
      const VkFormat& format,
      const VkExtent2D& extent,
      const VkComponentMapping& view_component_mapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY },
      bool copiable = false,
      const VkSampleCountFlagBits& sample_count = VK_SAMPLE_COUNT_1_BIT
    );

    // DELETES
    static std::shared_ptr<Attachment> Create(const AttachmentCreateInfo& info) = delete;
    static std::shared_ptr<Attachment> Create
    (
      const VkFormat& format,
      const AttachmentTypeFlags type,
      const VkExtent3D& extent,
      const VkComponentMapping& view_component_mapping = DEFAULT_VIEW_COMPONENT_MAPPING,
      uint32_t layer_count = 1,
      uint32_t mipmap_levels = 1,
      bool copiable = false,
      const VkSampleCountFlagBits& sample_count = VK_SAMPLE_COUNT_1_BIT
    ) = delete;
  };
}