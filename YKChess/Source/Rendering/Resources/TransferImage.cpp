#include "Rendering/Resources/TransferImage.h"

namespace yk
{

  TransferImage TransferImage::Create(Image& src_image, bool copy)
  {
    TransferImage& src_image_temp = static_cast<TransferImage&>(src_image);
    TransferImage image = TransferImage::Create(src_image_temp.m_Format, src_image_temp.m_Type, src_image_temp.m_Extent, src_image_temp.m_LayerCount, src_image_temp.m_MipmapLevels, src_image_temp.m_CreateFlags, src_image_temp.m_SampleCount);

    if (copy)
      src_image.CopyToImage(static_cast<Image&>(image));

    return image;
  }

  TransferImage TransferImage::Create(const TransferImageCreateInfo& info)
  {
    return TransferImage::Create(info.Format, info.Type, info.Extent, info.LayerCount, info.MipmapLevels, info.Flags, info.SampleCount);
  }

  TransferImage TransferImage::Create(const VkFormat& format, const VkImageType& type, const VkExtent3D& extent, const uint32_t& layer_count, const uint32_t& mipmap_levels, const VkImageCreateFlags& flags, const VkSampleCountFlagBits& sample_count)
  {
    TransferImage image; 
    image.CreateImage(format, type, extent, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, layer_count, mipmap_levels, flags, VK_IMAGE_TILING_OPTIMAL, sample_count);
    return image;
  }

}