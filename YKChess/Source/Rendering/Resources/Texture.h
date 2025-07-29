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

    static Texture Create(const std::filesystem::path& filepath, const std::shared_ptr<Sampler> sampler = nullptr);

  private:
    std::filesystem::path m_Filepath;
    std::shared_ptr<SampledImage> m_Image;
  };

}