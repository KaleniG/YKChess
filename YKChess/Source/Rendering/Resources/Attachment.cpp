#include <YKLib.h>

#include "Rendering/Resources/Attachment.h"

namespace yk
{

  std::shared_ptr<Attachment> Attachment::Create(const AttachmentCreateInfo& info)
  {
    return Attachment::Create(info.Format, info.Type, info.Extent, info.ComponentMapping, info.LayerCount, info.MipmapLevels, info.Copiable, info.SampleCount);
  }

  std::shared_ptr<Attachment> Attachment::Create(const VkFormat& format, const AttachmentTypeFlags type, const VkExtent2D& extent, const VkComponentMapping& view_component_mapping, uint32_t layer_count, uint32_t mipmap_levels, bool copiable, const VkSampleCountFlagBits& sample_count)
  {
    std::shared_ptr<Attachment> attachment = std::make_shared<Attachment>();

    attachment->CreateAttachment(format, type, extent, layer_count, mipmap_levels, copiable, sample_count);
    attachment->CreateImageView(view_component_mapping);
   
    return attachment;
  }

  void Attachment::CreateAttachment(const VkFormat& format, const AttachmentTypeFlags type, const VkExtent2D& extent, uint32_t layer_count, uint32_t mipmap_levels, bool copiable, const VkSampleCountFlagBits& sample_count)
  {
    m_AttachmentType = type;

    VkImageUsageFlags usage = 0;

    if (type & YK_ATTACHMENT_TYPE_COLOR_BIT)
    {
      YK_ASSERT(!(type & YK_ATTACHMENT_TYPE_DEPTH_STENCIL_BIT), "Vulkan/System: cannot have a color attachment that is also a depth/stencil attachment");
      usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }
    if (type & YK_ATTACHMENT_TYPE_DEPTH_STENCIL_BIT)
    {
      YK_ASSERT(!(type & YK_ATTACHMENT_TYPE_COLOR_BIT), "Vulkan/System: cannot have a depth/stencil attachment that is also a color attachment");
      usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }
    if (type & YK_ATTACHMENT_TYPE_RESOLVE_BIT)
    {
      YK_ASSERT(!(type & YK_ATTACHMENT_TYPE_DEPTH_STENCIL_BIT), "Vulkan/System: cannot have a resolve attachment that is also a depth/stencil attachment");
      YK_ASSERT(sample_count != VK_SAMPLE_COUNT_1_BIT, "System: a resolve attachment needs to have sample count 1");
      usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }
    if (type & YK_ATTACHMENT_TYPE_INPUT_BIT)
    {
      YK_ASSERT(!(type & YK_ATTACHMENT_TYPE_DEPTH_STENCIL_BIT), "Vulkan/System: cannot have a depth/stencil attachment that is also a color attachment");
      usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }
    if (type & YK_ATTACHMENT_TYPE_TRANSIENT_BIT)
    {
      YK_ASSERT(type & YK_ATTACHMENT_TYPE_COLOR_BIT & YK_ATTACHMENT_TYPE_DEPTH_STENCIL_BIT & YK_ATTACHMENT_TYPE_INPUT_BIT & YK_ATTACHMENT_TYPE_RESOLVE_BIT, "Vulkan/System: a transient attachment needs to be a color, input, resolve or depth/stebcil attachment");
      usage |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
    }

    if (copiable)
      usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    Image::CreateImage(format, VK_IMAGE_TYPE_2D, {extent.width, extent.height, 1}, usage, (type & YK_ATTACHMENT_TYPE_TRANSIENT_BIT ? VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT : 0) | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, layer_count, mipmap_levels, 0, VK_IMAGE_TILING_OPTIMAL, sample_count);
  }

}