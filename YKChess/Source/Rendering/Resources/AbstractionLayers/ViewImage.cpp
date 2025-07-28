#include <YKLib.h>

#include "Rendering/Resources/AbstractionLayers/ViewImage.h"
#include "Rendering/Renderer.h"
#include "Rendering/Utils.h"

namespace yk
{
  ViewImage::~ViewImage()
  {
    vkDestroyImageView(Renderer::GetDevice()->GetLogical(), m_View, VK_NULL_HANDLE);
  }

  void ViewImage::CreateImageView(const VkComponentMapping& view_component_mapping)
  {
    VkImageViewCreateInfo imageViewCreateInfo = {};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = m_Image;
    imageViewCreateInfo.format = m_Format;
    imageViewCreateInfo.subresourceRange.aspectMask = m_Aspect;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = m_MipmapLevels;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = m_LayerCount;
    imageViewCreateInfo.components = view_component_mapping;

    switch (m_Type)
    {
      case VK_IMAGE_TYPE_1D:
      {
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_1D;
        if (m_LayerCount > 1)
          imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
        break;
      }
      case VK_IMAGE_TYPE_2D:
      {
        if (m_CreateFlags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT)
        {
          imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
          if (m_LayerCount > 6)
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
          break;
        }
        else
        {
          imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
          if (m_LayerCount > 1)
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
          break;
        }
      }
      case VK_IMAGE_TYPE_3D:
      {
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;
        break;
      }
    }

    VkResult result = vkCreateImageView(Renderer::GetDevice()->GetLogical(), &imageViewCreateInfo, VK_NULL_HANDLE, &m_View);
    YK_ASSERT(result == VK_SUCCESS, "Vulkan: failed to create the image view. {}", Utils::VkResultToString(result));
  }
}