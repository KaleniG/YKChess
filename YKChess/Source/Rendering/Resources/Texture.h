#pragma once

#include <filesystem>
#include <memory>

#include "Rendering/Resources/SampledImage.h"

namespace yk
{

  class Texture
  {
  public:
    const std::shared_ptr<SampledImage>& GetImage() const { return m_Image; }
    int32_t GetWidth() const { return m_Width; }
    int32_t GetHeight() const { return m_Height; }

    static std::shared_ptr<Texture> Create(const std::filesystem::path& filepath, const std::shared_ptr<Sampler> sampler = nullptr);

  private:
    int32_t m_Width = 0;
    int32_t m_Height = 0;
    std::filesystem::path m_Filepath;
    std::shared_ptr<SampledImage> m_Image;
  };

}