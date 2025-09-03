#include <YKLib.h>

#include "Rendering/ImageResource.h"
#include "Rendering/Renderer.h"

namespace yk
{
  const SubTexture& ImageResource::GetSubTexture(uint32_t index) const
  {
    YK_ASSERT(index < m_SubTextures.size(), "Index is out of bounds");
    return m_SubTextures[index];
  }

  void ImageResource::SetSlot(std::shared_ptr<ImageResource> image, uint32_t slot)
  {
    image->m_Slot = slot;
    Renderer::SetImageSlot(slot, image);
  }

  std::shared_ptr<ImageResource> ImageResource::Create(const std::filesystem::path& image_filepath, uint32_t slot, int32_t x_cut_size /*= -1*/, int32_t y_cut_size /*= -1*/)
  {
    std::shared_ptr<ImageResource> imageResource = std::make_shared<ImageResource>();

    imageResource->m_Slot = slot;

    imageResource->m_Texture = Texture::Create(image_filepath);

    YK_ASSERT(x_cut_size <= imageResource->m_Texture->GetHeight(), "X axis cut size larger than the image height");
    YK_ASSERT(y_cut_size <= imageResource->m_Texture->GetWidth(), "Y axis cut size larger than the image width");

    if (x_cut_size == -1)
      x_cut_size = imageResource->m_Texture->GetHeight();

    if (y_cut_size == -1)
      y_cut_size = imageResource->m_Texture->GetWidth();

    int32_t horizontalCuts = imageResource->m_Texture->GetWidth() / x_cut_size;
    int32_t verticalCuts = imageResource->m_Texture->GetHeight() / y_cut_size;

    imageResource->m_SubTextures.reserve(horizontalCuts * verticalCuts);

    for (int y = 0; y < verticalCuts; ++y)
    {
      for (int x = 0; x < horizontalCuts; ++x)
      {
        float texWidth = static_cast<float>(imageResource->m_Texture->GetWidth());
        float texHeight = static_cast<float>(imageResource->m_Texture->GetHeight());

        const float litlOffset = 0.1f; // When zoomed in the subtexture cut-outs show fragments of other subtextures so we go 0.1f inside the texture (lowering the value will result in unjustly cut textures)
        float u0 = (x * x_cut_size + litlOffset) / texWidth;
        float v0 = (y * y_cut_size + litlOffset) / texHeight;
        float u1 = ((x + 1) * x_cut_size - litlOffset) / texWidth;
        float v1 = ((y + 1) * y_cut_size - litlOffset) / texHeight;

        std::array<glm::vec2, 4> uv = 
        {
          glm::vec2(u0, v0),
          glm::vec2(u1, v0),
          glm::vec2(u1, v1),
          glm::vec2(u0, v1)
        };

        imageResource->m_SubTextures.emplace_back(uv, slot);
      }
    }

    Renderer::SetImageSlot(slot, imageResource);
    return imageResource;
  }
}