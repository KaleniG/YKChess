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
    uint32_t TextureSlot;

    SubTexture() = default;
    SubTexture(const std::array<glm::vec2, 4>& uv, uint32_t slot)
      : UVCoordinates(uv), TextureSlot(slot) {}
  };

  class ImageResource
  {
  public:
    std::shared_ptr<Texture> GetTexture() const { return m_Texture; }
    const SubTexture& GetSubTexture(uint32_t index = 0) const;
    uint32_t GetSubTextureCount() const { return m_SubTextures.size(); }

    static void SetSlot(std::shared_ptr<ImageResource> image, uint32_t slot);
    uint32_t GetSlot() const { return m_Slot; }

    static std::shared_ptr<ImageResource> Create(const std::filesystem::path& image_filepath, uint32_t slot, int32_t x_cut_size = -1, int32_t y_cut_size = -1);

  private:
    uint32_t m_Slot;
    std::shared_ptr<Texture> m_Texture;
    std::vector<SubTexture> m_SubTextures;
  };
}