#include "Rendering/Resources/ResolveAttachment.h"

namespace yk
{

  std::shared_ptr<ResolveAttachment> ResolveAttachment::Create(const ResolveAttachmentCreateInfo& info)
  {
    return ResolveAttachment::Create(info.Format, info.Extent, info.ComponentMapping, info.Copiable);
  }

  std::shared_ptr<ResolveAttachment> ResolveAttachment::Create(const VkFormat& format, const VkExtent2D& extent, const VkComponentMapping& view_component_mapping, bool copiable)
  {
    std::shared_ptr<ResolveAttachment> attachment = std::make_shared<ResolveAttachment>();

    attachment->CreateAttachment(format, YK_ATTACHMENT_TYPE_DEPTH_STENCIL_BIT, extent, 1, 1, copiable, VK_SAMPLE_COUNT_1_BIT);
    attachment->CreateImageView(view_component_mapping);

    return attachment;
  }

}