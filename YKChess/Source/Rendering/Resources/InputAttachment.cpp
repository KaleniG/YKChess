#include "Rendering/Resources/InputAttachment.h"
#include "Rendering/Renderer.h"
#include "Rendering/Utils.h"

namespace yk
{
  std::shared_ptr<InputAttachment> InputAttachment::Create(const InputAttachmentCreateInfo& info)
  {
    return InputAttachment::Create(info.Format, info.Extent, info.ComponentMapping, info.LayerCount, info.MipmapLevels, info.Copiable, info.SampleCount);
  }

  std::shared_ptr<InputAttachment> InputAttachment::Create(const VkFormat& format, const VkExtent2D& extent, const VkComponentMapping& view_component_mapping, uint32_t layer_count, uint32_t mipmap_levels, bool copiable, const VkSampleCountFlagBits& sample_count)
  {
    std::shared_ptr<InputAttachment> attachment = std::shared_ptr<InputAttachment>();

    attachment->CreateAttachment(format, YK_ATTACHMENT_TYPE_INPUT_BIT, extent, layer_count, mipmap_levels, copiable, sample_count);
    attachment->CreateImageView(view_component_mapping);

    return attachment;
  }
}