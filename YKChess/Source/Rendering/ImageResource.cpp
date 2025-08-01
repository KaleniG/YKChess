#include <YKLib.h>

#include "Rendering/ImageResource.h"

namespace yk
{

  const SubTexture& ImageResource::GetSubTexture(uint32_t index) const
  {
    YK_ASSERT(index < m_SubTextures.size(), "Index is out of bounds");
    return m_SubTextures[index];
  }

  std::shared_ptr<ImageResource> ImageResource::Create(const std::filesystem::path& image_filepath, int32_t x_cut_size /*= -1*/, int32_t y_cut_size /*= -1*/)
  {
    std::shared_ptr<ImageResource> imageResource = std::make_shared<ImageResource>();

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
        float u0 = static_cast<float>(x * x_cut_size) / imageResource->m_Texture->GetWidth();
        float v0 = static_cast<float>(y * y_cut_size) / imageResource->m_Texture->GetHeight();
        float u1 = static_cast<float>((x + 1) * x_cut_size) / imageResource->m_Texture->GetWidth();
        float v1 = static_cast<float>((y + 1) * y_cut_size) / imageResource->m_Texture->GetHeight();

        std::array<glm::vec2, 4> uv = 
        {
          glm::vec2(u0, v0),
          glm::vec2(u1, v0),
          glm::vec2(u1, v1),
          glm::vec2(u0, v1)
        };

        imageResource->m_SubTextures.emplace_back(uv);
      }
    }

    return imageResource;
  }
}