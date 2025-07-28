#include <YKLib.h>

#include "Rendering/Resources/AbstractionLayers/Image.h"
#include "Rendering/Resources/AbstractionLayers/Buffer.h"
#include "Rendering/Renderer.h"
#include "Rendering/Utils.h"

// New assertion macros
#define ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, usage, valid_access, valid_usages) if (access & valid_access) YK_ASSERT(usage & valid_usages, "Vulkan/System: invalid access mask for this resource's usage");
#define ASSERT_IMAGE_ASPECT_FOR_IMAGE_LAYOUT(layout, aspect, valid_layout, valid_aspect) if (layout == valid_layout) YK_ASSERT(aspect & valid_aspect, "Vulkan/System: invalid layout for this image's color aspect");
#define ASSERT_ACCESS_MASKS(access, valid_mask) YK_ASSERT((access) & (valid_mask), "Vulkan/System: invalid access masks specified during image layout transition");

namespace yk
{
  namespace Utils
  {
    static VkImageAspectFlags FormatToAspectMask(const VkFormat& format)
    {
      switch (format)
      {
        // Depth-only formats
      case VK_FORMAT_D16_UNORM:
      case VK_FORMAT_D32_SFLOAT:
      case VK_FORMAT_X8_D24_UNORM_PACK32:
      {
        return VK_IMAGE_ASPECT_DEPTH_BIT;
        break;
      }

      // Stencil-only formats
      case VK_FORMAT_S8_UINT:
      {
        return VK_IMAGE_ASPECT_STENCIL_BIT;
        break;
      }

      // Depth-stencil formats
      case VK_FORMAT_D16_UNORM_S8_UINT:
      case VK_FORMAT_D32_SFLOAT_S8_UINT:
      {
        return (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
        break;
      }

      // Color formats
      case VK_FORMAT_R4G4_UNORM_PACK8:
      case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
      case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
      case VK_FORMAT_R5G6B5_UNORM_PACK16:
      case VK_FORMAT_B5G6R5_UNORM_PACK16:
      case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
      case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
      case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
      case VK_FORMAT_R8_UNORM:
      case VK_FORMAT_R8_SNORM:
      case VK_FORMAT_R8_USCALED:
      case VK_FORMAT_R8_SSCALED:
      case VK_FORMAT_R8_UINT:
      case VK_FORMAT_R8_SINT:
      case VK_FORMAT_R8_SRGB:
      case VK_FORMAT_R8G8_UNORM:
      case VK_FORMAT_R8G8_SNORM:
      case VK_FORMAT_R8G8_USCALED:
      case VK_FORMAT_R8G8_SSCALED:
      case VK_FORMAT_R8G8_UINT:
      case VK_FORMAT_R8G8_SINT:
      case VK_FORMAT_R8G8_SRGB:
      case VK_FORMAT_R8G8B8_UNORM:
      case VK_FORMAT_R8G8B8_SNORM:
      case VK_FORMAT_R8G8B8_USCALED:
      case VK_FORMAT_R8G8B8_SSCALED:
      case VK_FORMAT_R8G8B8_UINT:
      case VK_FORMAT_R8G8B8_SINT:
      case VK_FORMAT_R8G8B8_SRGB:
      case VK_FORMAT_B8G8R8_UNORM:
      case VK_FORMAT_B8G8R8_SNORM:
      case VK_FORMAT_B8G8R8_USCALED:
      case VK_FORMAT_B8G8R8_SSCALED:
      case VK_FORMAT_B8G8R8_UINT:
      case VK_FORMAT_B8G8R8_SINT:
      case VK_FORMAT_B8G8R8_SRGB:
      case VK_FORMAT_R8G8B8A8_UNORM:
      case VK_FORMAT_R8G8B8A8_SNORM:
      case VK_FORMAT_R8G8B8A8_USCALED:
      case VK_FORMAT_R8G8B8A8_SSCALED:
      case VK_FORMAT_R8G8B8A8_UINT:
      case VK_FORMAT_R8G8B8A8_SINT:
      case VK_FORMAT_R8G8B8A8_SRGB:
      case VK_FORMAT_B8G8R8A8_UNORM:
      case VK_FORMAT_B8G8R8A8_SNORM:
      case VK_FORMAT_B8G8R8A8_USCALED:
      case VK_FORMAT_B8G8R8A8_SSCALED:
      case VK_FORMAT_B8G8R8A8_UINT:
      case VK_FORMAT_B8G8R8A8_SINT:
      case VK_FORMAT_B8G8R8A8_SRGB:
      case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
      case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
      case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
      case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
      case VK_FORMAT_A8B8G8R8_UINT_PACK32:
      case VK_FORMAT_A8B8G8R8_SINT_PACK32:
      case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
      case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
      case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
      case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
      case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
      case VK_FORMAT_A2R10G10B10_UINT_PACK32:
      case VK_FORMAT_A2R10G10B10_SINT_PACK32:
      case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
      case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
      case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
      case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
      case VK_FORMAT_A2B10G10R10_UINT_PACK32:
      case VK_FORMAT_A2B10G10R10_SINT_PACK32:
      case VK_FORMAT_R16_UNORM:
      case VK_FORMAT_R16_SNORM:
      case VK_FORMAT_R16_USCALED:
      case VK_FORMAT_R16_SSCALED:
      case VK_FORMAT_R16_UINT:
      case VK_FORMAT_R16_SINT:
      case VK_FORMAT_R16_SFLOAT:
      case VK_FORMAT_R16G16_UNORM:
      case VK_FORMAT_R16G16_SNORM:
      case VK_FORMAT_R16G16_USCALED:
      case VK_FORMAT_R16G16_SSCALED:
      case VK_FORMAT_R16G16_UINT:
      case VK_FORMAT_R16G16_SINT:
      case VK_FORMAT_R16G16_SFLOAT:
      case VK_FORMAT_R16G16B16_UNORM:
      case VK_FORMAT_R16G16B16_SNORM:
      case VK_FORMAT_R16G16B16_USCALED:
      case VK_FORMAT_R16G16B16_SSCALED:
      case VK_FORMAT_R16G16B16_UINT:
      case VK_FORMAT_R16G16B16_SINT:
      case VK_FORMAT_R16G16B16_SFLOAT:
      case VK_FORMAT_R16G16B16A16_UNORM:
      case VK_FORMAT_R16G16B16A16_SNORM:
      case VK_FORMAT_R16G16B16A16_USCALED:
      case VK_FORMAT_R16G16B16A16_SSCALED:
      case VK_FORMAT_R16G16B16A16_UINT:
      case VK_FORMAT_R16G16B16A16_SINT:
      case VK_FORMAT_R16G16B16A16_SFLOAT:
      case VK_FORMAT_R32_UINT:
      case VK_FORMAT_R32_SINT:
      case VK_FORMAT_R32_SFLOAT:
      case VK_FORMAT_R32G32_UINT:
      case VK_FORMAT_R32G32_SINT:
      case VK_FORMAT_R32G32_SFLOAT:
      case VK_FORMAT_R32G32B32_UINT:
      case VK_FORMAT_R32G32B32_SINT:
      case VK_FORMAT_R32G32B32_SFLOAT:
      case VK_FORMAT_R32G32B32A32_UINT:
      case VK_FORMAT_R32G32B32A32_SINT:
      case VK_FORMAT_R32G32B32A32_SFLOAT:
      case VK_FORMAT_R64_UINT:
      case VK_FORMAT_R64_SINT:
      case VK_FORMAT_R64_SFLOAT:
      case VK_FORMAT_R64G64_UINT:
      case VK_FORMAT_R64G64_SINT:
      case VK_FORMAT_R64G64_SFLOAT:
      case VK_FORMAT_R64G64B64_UINT:
      case VK_FORMAT_R64G64B64_SINT:
      case VK_FORMAT_R64G64B64_SFLOAT:
      case VK_FORMAT_R64G64B64A64_UINT:
      case VK_FORMAT_R64G64B64A64_SINT:
      case VK_FORMAT_R64G64B64A64_SFLOAT:
      {
        return VK_IMAGE_ASPECT_COLOR_BIT;
        break;
      }

      // Undefined and Compressed formats
      default:
        YK_ASSERT(false, "Vulkan/System: invalid or not yet implemented image format specified");
      }
    }
  }

  Image::~Image()
  {
    vkDestroyImage(Renderer::GetDevice()->GetLogical(), m_Image, VK_NULL_HANDLE);
  }

  void Image::Transition(const VkImageLayout& new_layout, const ImageTransitionSpecifics& specifics)
  {
    if (m_CurrentLayout == new_layout)
    {
      YK_WARN("Vulkan/System: transitioning the image to the same layout it is at the moment");
      return;
    }

    Image::ImageLayoutToAspectCheck(new_layout);

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = m_CurrentLayout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_Image;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
    barrier.subresourceRange.aspectMask = m_Aspect;
    barrier.srcAccessMask = m_CurrentAccessMask;
    barrier.dstAccessMask = Image::ImageLayoutToAccessMask(new_layout, specifics.AccessMask);

    Image::AccessMaskToImageUsageCheck(barrier.dstAccessMask);

    VkPipelineStageFlags dstStages = Resource::AccessMaskToPipelineStages(barrier.dstAccessMask, specifics.PipelineStagesMask);

    VkCommandBuffer commandBuffer = Renderer::GetDevice()->GetSingleTimeCommandBuffer();
    vkCmdPipelineBarrier(commandBuffer, m_CurrentPipelineStageMask, dstStages, 0, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, 1, &barrier);
    Renderer::GetDevice()->SubmitSingleTimeCommandBuffer(commandBuffer);

    m_CurrentLayout = new_layout;
    m_CurrentAccessMask = barrier.dstAccessMask;
    m_CurrentPipelineStageMask = dstStages;

    YK_INFO("System: image '{}' transitioned", (int)m_Image);
  }

  void Image::CopyToImage(Image& dst_image, const ImageToImageCopySpecifics& specifics)
  {
    YK_ASSERT(m_Usage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT, "Vulkan/System: the source image cannot be used as a data transfer source, 'VK_IMAGE_USAGE_TRANSFER_SRC_BIT' usage flag has not been specified during its image creation");
    YK_ASSERT(dst_image.m_Usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT, "Vulkan/System: the destination image cannot be used as a data transfer destination, 'VK_IMAGE_USAGE_TRANSFER_DST_BIT' usage flag has not been specified during the image creation");

    if (m_CurrentLayout != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
      Transition(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    if (dst_image.m_CurrentLayout != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
      dst_image.Transition(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkCommandBuffer commandBuffer = Renderer::GetDevice()->GetSingleTimeCommandBuffer();
    if (specifics.CopyRegions.size())
    {
      for (const VkImageCopy& copyRegion : specifics.CopyRegions)
      {
        YK_ASSERT(copyRegion.srcOffset.x + copyRegion.extent.width <= m_Extent.width && copyRegion.srcOffset.y + copyRegion.extent.height <= m_Extent.height && copyRegion.srcOffset.z + copyRegion.extent.depth <= m_Extent.depth, "Vulkan/System: copying off-range data from the source image");
        YK_ASSERT(copyRegion.dstOffset.x + copyRegion.extent.width <= dst_image.m_Extent.width && copyRegion.dstOffset.y + copyRegion.extent.height <= dst_image.m_Extent.height && copyRegion.dstOffset.z + copyRegion.extent.depth <= dst_image.m_Extent.depth, "Vulkan/System: copying to an area that is out of range of the destination image");
        YK_ASSERT(copyRegion.srcSubresource.baseArrayLayer + copyRegion.srcSubresource.layerCount <= m_LayerCount, "Vulkan/System: copying more layers than available in the source image");
        YK_ASSERT(copyRegion.dstSubresource.baseArrayLayer + copyRegion.dstSubresource.layerCount <= dst_image.m_LayerCount, "Vulkan/System: copying more layers than available in the destination image");
        YK_ASSERT(copyRegion.srcSubresource.mipLevel <= m_MipmapLevels, "Vulkan/System: specified mip level exceeds the available mip levels in the source image");
        YK_ASSERT(copyRegion.dstSubresource.mipLevel <= dst_image.m_MipmapLevels, "Vulkan/System: specified mip level exceeds the available mip levels in the destination image");
      }

      vkCmdCopyImage(commandBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst_image.m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(specifics.CopyRegions.size()), specifics.CopyRegions.data());
    }
    else
    {
      YK_ASSERT(m_Extent.width <= dst_image.m_Extent.width && m_Extent.height <= dst_image.m_Extent.height && m_Extent.depth <= dst_image.m_Extent.depth, "Vulkan/System: the source image size is greater than the destination image's size during whole image copy");
      if (m_Extent.width < dst_image.m_Extent.width || m_Extent.height < dst_image.m_Extent.height || m_Extent.depth < dst_image.m_Extent.depth)
        YK_WARN("Vulkan/System: executing complete copy of a source image (size:{0}x{1}x{2}) to a destination image with bigger size (size:{3}x{4}x{5})", m_Extent.width, m_Extent.height, m_Extent.depth, dst_image.m_Extent.width, dst_image.m_Extent.height, dst_image.m_Extent.depth);

      VkImageCopy region = {};
      region.srcSubresource.aspectMask = m_Aspect;
      region.srcSubresource.baseArrayLayer = 0;
      region.srcSubresource.layerCount = m_LayerCount;
      region.srcSubresource.mipLevel = 0;
      region.dstSubresource.aspectMask = dst_image.m_Aspect;
      region.dstSubresource.baseArrayLayer = 0;
      region.dstSubresource.layerCount = dst_image.m_LayerCount;
      region.dstSubresource.mipLevel = 0;
      region.srcOffset = { 0, 0, 0 };
      region.dstOffset = { 0, 0, 0 };
      region.extent = m_Extent;

      vkCmdCopyImage(commandBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst_image.m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    }
    Renderer::GetDevice()->SubmitSingleTimeCommandBuffer(commandBuffer);

    YK_INFO("System: image '{}' copied to image '{}'", (int)m_Image, (int)dst_image.m_Image);
  }

  void Image::CopyToImage(Image& dst_image, const VkImageCopy& copy_region, bool gen_mipmaps)
  {
    ImageToImageCopySpecifics specifics = {};
    specifics.CopyRegions = { copy_region };
    specifics.GenerateMipmaps = gen_mipmaps;

    Image::CopyToImage(dst_image, specifics);
  }

  void Image::CopyToBuffer(Buffer& dst_buffer, const ImageToBufferCopySpecifics& specifics)
  {
    YK_ASSERT(m_Usage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT, "Vulkan/System: the source image cannot be used as a data transfer source, 'VK_IMAGE_USAGE_TRANSFER_SRC_BIT' usage flag has not been specified during its image creation");
    YK_ASSERT(dst_buffer.m_Usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT, "Vulkan/System: the destination buffer cannot be used as a data transfer destination, 'VK_BUFFER_USAGE_TRANSFER_DST_BIT' usage flag has not been specified during buffer creation");

    if (m_CurrentLayout != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
      Transition(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    if (dst_buffer.m_CurrentAccessMask != VK_ACCESS_TRANSFER_WRITE_BIT)
      dst_buffer.Transition(VK_ACCESS_TRANSFER_WRITE_BIT);

    VkCommandBuffer commandBuffer = Renderer::GetDevice()->GetSingleTimeCommandBuffer();
    if (specifics.CopyData.size())
    {
      for (const VkBufferImageCopy& copyData : specifics.CopyData)
      {
        YK_ASSERT(copyData.bufferOffset + copyData.bufferRowLength * copyData.bufferImageHeight <= dst_buffer.m_Size, "Vulkan/System: copying off-range data to the destination buffer");
        YK_ASSERT(copyData.imageOffset.x + copyData.imageExtent.width <= m_Extent.width && copyData.imageOffset.y + copyData.imageExtent.height <= m_Extent.height && copyData.imageOffset.z + copyData.imageExtent.depth <= m_Extent.depth, "Vulkan/System: copying to an area that is out of range of the source image");

        YK_ASSERT(copyData.imageSubresource.baseArrayLayer + copyData.imageSubresource.layerCount <= m_LayerCount, "Vulkan/System: copying more layers than available in the source image");
        YK_ASSERT(copyData.imageSubresource.mipLevel <= m_MipmapLevels, "Vulkan/System: specified mip level exceeds the available mip levels in the source image");
      }

      vkCmdCopyImageToBuffer(commandBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst_buffer.m_Buffer, static_cast<uint32_t>(specifics.CopyData.size()), specifics.CopyData.data());
    }
    else
    {
      VkBufferImageCopy region = {};
      region.bufferOffset = 0;
      region.bufferRowLength = 0;
      region.bufferImageHeight = 0;
      region.imageSubresource.aspectMask = m_Aspect;
      region.imageSubresource.baseArrayLayer = 0;
      region.imageSubresource.layerCount = m_LayerCount;
      region.imageSubresource.mipLevel = 0;
      region.imageOffset = { 0, 0, 0 };
      region.imageExtent = m_Extent;

      vkCmdCopyImageToBuffer(commandBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst_buffer.m_Buffer, 1, &region);
    }
    Renderer::GetDevice()->SubmitSingleTimeCommandBuffer(commandBuffer);

    YK_INFO("System: image '{}' copied to buffer '{}'", (int)m_Image, (int)dst_buffer.m_Buffer);
  }

  void Image::CopyToBuffer(Buffer& dst_buffer, const VkBufferImageCopy& copy_data)
  {
    ImageToBufferCopySpecifics specifics = {};
    specifics.CopyData = { copy_data };
    Image::CopyToBuffer(dst_buffer, specifics);
  }

  void Image::ImageLayoutToAspectCheck(const VkImageLayout& layout)
  {
    ASSERT_IMAGE_ASPECT_FOR_IMAGE_LAYOUT(layout, m_Aspect, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
    ASSERT_IMAGE_ASPECT_FOR_IMAGE_LAYOUT(layout, m_Aspect, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
  }

  void Image::AccessMaskToImageUsageCheck(const VkAccessFlags& access)
  {
    ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, m_Usage, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT);
    ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, m_Usage, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_USAGE_STORAGE_BIT);
    ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, m_Usage, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, m_Usage, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, m_Usage, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, m_Usage, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, m_Usage, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
    ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, m_Usage, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_USAGE_TRANSFER_DST_BIT);
    ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, m_Usage, VK_ACCESS_INPUT_ATTACHMENT_READ_BIT, VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
  }

  VkAccessFlags Image::ImageLayoutToAccessMask(const VkImageLayout& layout, const VkAccessFlags& access)
  {
    VkAccessFlags accessMask = VK_ACCESS_NONE;

    switch (layout)
    {
    case VK_IMAGE_LAYOUT_UNDEFINED:
    {
      break;
    }
    case VK_IMAGE_LAYOUT_GENERAL:
    {
      ASSERT_ACCESS_MASKS(access,
        VK_ACCESS_INDIRECT_COMMAND_READ_BIT |
        VK_ACCESS_INDEX_READ_BIT |
        VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT |
        VK_ACCESS_UNIFORM_READ_BIT |
        VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
        VK_ACCESS_SHADER_READ_BIT |
        VK_ACCESS_SHADER_WRITE_BIT |
        VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
        VK_ACCESS_TRANSFER_READ_BIT |
        VK_ACCESS_TRANSFER_WRITE_BIT |
        VK_ACCESS_HOST_READ_BIT |
        VK_ACCESS_HOST_WRITE_BIT |
        VK_ACCESS_MEMORY_READ_BIT |
        VK_ACCESS_MEMORY_WRITE_BIT
      );
      accessMask |= access;
      break;
    }
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
    {
      if (access != VK_ACCESS_NONE)
      {
        ASSERT_ACCESS_MASKS(access,
          VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
          VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
          VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
          VK_ACCESS_MEMORY_READ_BIT
        );
        accessMask |= access;
      }
      else
        accessMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      break;
    }
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
    {
      if (access != VK_ACCESS_NONE)
      {
        ASSERT_ACCESS_MASKS(access,
          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
          VK_ACCESS_MEMORY_READ_BIT |
          VK_ACCESS_MEMORY_WRITE_BIT
        );
        accessMask |= access;
      }
      else
        accessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
      break;
    }
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
    {
      if (access != VK_ACCESS_NONE)
      {
        ASSERT_ACCESS_MASKS(access,
          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
          VK_ACCESS_SHADER_READ_BIT |
          VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
          VK_ACCESS_MEMORY_READ_BIT
        );
        accessMask |= access;
      }
      else
        accessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
      break;
    }
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
    {
      if (access != VK_ACCESS_NONE)
      {
        ASSERT_ACCESS_MASKS(access,
          VK_ACCESS_SHADER_READ_BIT |
          VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
          VK_ACCESS_MEMORY_READ_BIT
        );
        accessMask |= access;
      }
      else
        accessMask |= VK_ACCESS_SHADER_READ_BIT;
      break;
    }
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
    {
      if (access != VK_ACCESS_NONE)
      {
        ASSERT_ACCESS_MASKS(access,
          VK_ACCESS_TRANSFER_READ_BIT |
          VK_ACCESS_MEMORY_READ_BIT
        );
        accessMask |= access;
      }
      else
        accessMask |= VK_ACCESS_TRANSFER_READ_BIT;
      break;
    }
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
    {
      if (access != VK_ACCESS_NONE)
      {
        ASSERT_ACCESS_MASKS(access,
          VK_ACCESS_TRANSFER_WRITE_BIT |
          VK_ACCESS_MEMORY_WRITE_BIT
        );
        accessMask |= access;
      }
      else
        accessMask |= VK_ACCESS_TRANSFER_WRITE_BIT;
      break;
    }
    case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
    case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
    {
      if (access != VK_ACCESS_NONE)
      {
        ASSERT_ACCESS_MASKS(access,
          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
          VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
          VK_ACCESS_SHADER_READ_BIT
        );
        accessMask |= access;
      }
      else
        accessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
      break;
    }
    case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
    {
      if (access != VK_ACCESS_NONE)
      {
        ASSERT_ACCESS_MASKS(access,
          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
          VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
          VK_ACCESS_SHADER_READ_BIT
        );
        accessMask |= access;
      }
      else
        accessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
      break;
    }
    case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL:
    {
      if (access != VK_ACCESS_NONE)
      {
        ASSERT_ACCESS_MASKS(access,
          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
          VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
          VK_ACCESS_SHADER_READ_BIT
        );
        accessMask |= access;
      }
      else
        accessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
      break;
    }
    case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL:
    {
      if (access != VK_ACCESS_NONE)
      {
        ASSERT_ACCESS_MASKS(access,
          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
          VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
          VK_ACCESS_SHADER_READ_BIT
        );
        accessMask |= access;
      }
      else
        accessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
      break;
    }
    case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL:
    {
      if (access != VK_ACCESS_NONE)
      {
        ASSERT_ACCESS_MASKS(access,
          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
          VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
          VK_ACCESS_SHADER_READ_BIT
        );
        accessMask |= access;
      }
      else
        accessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
      break;
    }
    case VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL:
    {
      ASSERT_ACCESS_MASKS(access,
        VK_ACCESS_SHADER_READ_BIT |
        VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
        VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
        VK_ACCESS_TRANSFER_READ_BIT
      );
      accessMask |= access;
      break;
    }
    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
    {
      ASSERT_ACCESS_MASKS(access,
        VK_ACCESS_MEMORY_READ_BIT |
        VK_ACCESS_MEMORY_WRITE_BIT
      );
      accessMask |= access;
      break;
    }
    case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR:
    {
      ASSERT_ACCESS_MASKS(access,
        VK_ACCESS_MEMORY_READ_BIT |
        VK_ACCESS_MEMORY_WRITE_BIT |
        VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
        VK_ACCESS_TRANSFER_READ_BIT |
        VK_ACCESS_TRANSFER_WRITE_BIT
      );
      accessMask |= access;
      break;
    }
    case VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT:
    {
      if (access != VK_ACCESS_NONE)
      {
        ASSERT_ACCESS_MASKS(access,
          VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT |
          VK_ACCESS_SHADER_READ_BIT |
          VK_ACCESS_TRANSFER_READ_BIT |
          VK_ACCESS_TRANSFER_WRITE_BIT
        );
        accessMask |= access;
      }
      else
        accessMask |= VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT;
      break;
    }
    case VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR:
    {
      if (access != VK_ACCESS_NONE)
      {
        ASSERT_ACCESS_MASKS(access,
          VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR |
          VK_ACCESS_SHADER_READ_BIT |
          VK_ACCESS_TRANSFER_READ_BIT |
          VK_ACCESS_TRANSFER_WRITE_BIT
        );
        accessMask |= access;
      }
      else
        accessMask |= VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR;
      break;
    }
    default:
    {
      YK_ASSERT(false, "Vulkan/System: unsupported image layout");
      break;
    }
    }

    return accessMask;
  }

  void Image::CreateImage(const VkFormat& format, const VkImageType& type, const VkExtent3D& extent, const VkImageUsageFlags& usage, const VkMemoryPropertyFlags& memory_properties, const uint32_t& layer_count, const uint32_t& mipmap_levels, const VkImageCreateFlags& flags, const VkImageTiling& tiling, const VkSampleCountFlagBits& sample_count)
  {
    switch (type)
    {
      case VK_IMAGE_TYPE_1D:
      {
        YK_ASSERT(extent.height == 1 && extent.depth == 1, "Vulkan/System: invalid height/depth specified for a 1D Image");
        break;
      }
      case VK_IMAGE_TYPE_2D:
      {
        YK_ASSERT(extent.depth == 1, "Vulkan/System: invalid depth specified for a 2D Image");
        break;
      }
      case VK_IMAGE_TYPE_3D:
      {
        YK_ASSERT(layer_count == 1, "Vulkan/System: invalid layer count specified for a 3D Image");
        break;
      }
      default:
      {
        YK_ASSERT(false, "Vulkan/System: unsupported image type specified");
        break;
      }
    }

    m_Format = format;
    m_Type = type;
    m_Extent = extent;
    m_LayerCount = layer_count;
    m_Tiling = tiling;
    m_MipmapLevels = mipmap_levels;
    m_Usage = usage;
    m_CreateFlags = flags;
    m_SampleCount = sample_count;

    if (flags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT)
      YK_ASSERT(layer_count % 6 == 0, "Vulkan/System: if the image is a cubemap, the layer count should be a multiple of 6");

    m_Aspect = Utils::FormatToAspectMask(format);
    m_Size = Utils::FormatToByteSize(format) * extent.width * extent.height * extent.depth * layer_count;

    VkFormatFeatureFlags formatFeatures = (tiling == VK_IMAGE_TILING_LINEAR) ? Renderer::GetDevice()->GetFormatProperties(format).linearTilingFeatures : Renderer::GetDevice()->GetFormatProperties(format).optimalTilingFeatures;
    if (usage & VK_IMAGE_USAGE_SAMPLED_BIT)
      YK_ASSERT(formatFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT, "Vulkan/System: the specified format for this image does not support image sampling");
    if (usage & VK_IMAGE_USAGE_STORAGE_BIT)
      YK_ASSERT(formatFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT, "Vulkan/System: the specified format for this image does not support image unfiltered read/write");

    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.flags = flags;
    imageCreateInfo.imageType = type;
    imageCreateInfo.extent = extent;
    imageCreateInfo.mipLevels = mipmap_levels;
    imageCreateInfo.arrayLayers = layer_count;
    imageCreateInfo.format = format;
    imageCreateInfo.tiling = tiling;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = usage;
    imageCreateInfo.samples = sample_count;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateImage(Renderer::GetDevice()->GetLogical(), &imageCreateInfo, VK_NULL_HANDLE, &m_Image);
    YK_ASSERT(result == VK_SUCCESS, "Vulkan: failed to create the image. {}", Utils::VkResultToString(result));

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(Renderer::GetDevice()->GetLogical(), m_Image, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocInfo = {};
    memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocInfo.allocationSize = memoryRequirements.size;
    memoryAllocInfo.memoryTypeIndex = Renderer::GetDevice()->FindMemoryTypeIndex(memoryRequirements.memoryTypeBits, memory_properties);

    result = vkAllocateMemory(Renderer::GetDevice()->GetLogical(), &memoryAllocInfo, VK_NULL_HANDLE, &m_Memory);
    YK_ASSERT(result == VK_SUCCESS, "Vulkan: failed to allocate memory for the image. {}", Utils::VkResultToString(result));

    result = vkBindImageMemory(Renderer::GetDevice()->GetLogical(), m_Image, m_Memory, 0);
    YK_ASSERT(result == VK_SUCCESS, "Vulkan: failed to bind the image memory. {}", Utils::VkResultToString(result));
  }
}