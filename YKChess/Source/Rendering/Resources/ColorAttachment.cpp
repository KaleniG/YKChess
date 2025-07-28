#include "Rendering/Resources/ColorAttachment.h"

namespace yk
{
  std::shared_ptr<ColorAttachment> ColorAttachment::Create(const ColorAttachmentCreateInfo& info)
  {
    return ColorAttachment::Create(info.Format, info.Extent, info.ComponentMapping, info.Copiable, info.SampleCount);
  }

  std::shared_ptr<ColorAttachment> ColorAttachment::Create(const VkFormat& format, const VkExtent2D& extent, const VkComponentMapping& view_component_mapping, bool copiable, const VkSampleCountFlagBits& sample_count)
  {
    std::shared_ptr<ColorAttachment> attachment = std::shared_ptr<ColorAttachment>();

    attachment->CreateAttachment(format, YK_ATTACHMENT_TYPE_COLOR_BIT, extent, 1, 1, copiable, sample_count);
    attachment->CreateImageView(view_component_mapping);

    return attachment;
  }
}