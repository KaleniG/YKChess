#include "Rendering/Resources/StorageImage.h"
#include "Rendering/Renderer.h"
#include "Rendering/Utils.h"

namespace yk
{

  StorageImage StorageImage::Create(const StorageImageCreateInfo& info)
  {
    return StorageImage::Create(info.Format, info.Type, info.Extent, info.ComponentMapping, info.LayerCount, info.MipmapLevels, info.Copiable, info.Flags, info.SampleCount);
  }

  StorageImage StorageImage::Create(const VkFormat& format, const VkImageType& type, const VkExtent3D& extent, const VkComponentMapping& view_component_mapping, const uint32_t& layer_count, const uint32_t& mipmap_levels, bool copiable, const VkImageCreateFlags& flags, const VkSampleCountFlagBits& sample_count)
  {
    StorageImage image;
    image.CreateImage(format, type, extent, (copiable ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : 0) | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, layer_count, mipmap_levels, flags, VK_IMAGE_TILING_OPTIMAL, sample_count);
    image.CreateImageView(view_component_mapping);
    return image;
  }

}