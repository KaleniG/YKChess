#include "Rendering/Resources/DepthStencilAttachment.h"

namespace yk
{
  std::shared_ptr<DepthStencilAttachment> DepthStencilAttachment::Create(const DepthStencilAttachmentCreateInfo& info)
  {
    return DepthStencilAttachment::Create(info.Format, info.Extent, info.ComponentMapping, info.Copiable, info.SampleCount);
  }

  std::shared_ptr<DepthStencilAttachment> DepthStencilAttachment::Create(const VkFormat& format, const VkExtent2D& extent, const VkComponentMapping& view_component_mapping, bool copiable, const VkSampleCountFlagBits& sample_count)
  {
    std::shared_ptr<DepthStencilAttachment> attachment = std::shared_ptr<DepthStencilAttachment>();

    attachment->CreateAttachment(format, YK_ATTACHMENT_TYPE_DEPTH_STENCIL_BIT, extent, 1, 1, copiable, sample_count);
    attachment->CreateImageView(view_component_mapping);

    return attachment;
  }
}