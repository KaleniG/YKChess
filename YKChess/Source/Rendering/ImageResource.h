#pragma once

#include <filesystem>
#include <memory>
#include <array>

#include <glm/glm.hpp>

#include "Rendering/Resources/Texture.h"

namespace yk
{
  struct SubTexture
  {
    std::array<glm::vec2, 4> UVCoordinates;

    SubTexture(const std::array<glm::vec2, 4>& uv)
      : UVCoordinates(uv) {}
  };

  class ImageResource
  {
  public:
    std::shared_ptr<Texture> GetTexture() const { return m_Texture; }
    const SubTexture& GetSubTexture(uint32_t index) const;
    uint32_t GetSubTextureCount() const { return m_SubTextures.size(); }

    static std::shared_ptr<ImageResource> Create(const std::filesystem::path& image_filepath, int32_t x_cut_size = -1, int32_t y_cut_size = -1);

  private:
    std::shared_ptr<Texture> m_Texture;
    std::vector<SubTexture> m_SubTextures;
  };
}