#pragma once

#include "Rendering/Resources/AbstractionLayers/Image.h"

#define DEFAULT_VIEW_COMPONENT_MAPPING { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY }

namespace yk
{

  class ViewImage : public Image
  {
  public:
    ~ViewImage();

    const VkImageView& GetView() const { return m_View; }

  protected:
    void CreateImageView(const VkComponentMapping& view_component_mapping);

  protected:
    VkImageView m_View = VK_NULL_HANDLE;
  };

}