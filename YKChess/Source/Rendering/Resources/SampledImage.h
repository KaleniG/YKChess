#pragma once

#include "Rendering/Resources/AbstractionLayers/ViewImage.h"
#include "Rendering/Resources/Sampler.h"

namespace yk
{

  struct SamplerConfig
  {
    std::shared_ptr<Sampler> Sampler = nullptr;
    SamplerCreateInfo SamplerCreateInfo = {};
  };

  struct SampledImageCreateInfo
  {
    VkFormat Format;
    VkImageType Type;
    VkExtent3D Extent;
    VkComponentMapping ComponentMapping = YK_DEFAULT_VIEW_COMPONENT_MAPPING;
    uint32_t LayerCount = 1;
    uint32_t MipmapLevels = 1;
    bool Copiable = false;
    VkImageCreateFlags Flags = 0;
    VkSampleCountFlagBits SampleCount = VK_SAMPLE_COUNT_1_BIT;
    SamplerConfig Sampler = {};
  };

  class SampledImage : public ViewImage
  {
  public:
    const std::shared_ptr<Sampler>& GetSampler() const { return r_Sampler; }

    static std::shared_ptr<SampledImage> Create(const SampledImageCreateInfo& info);
    static std::shared_ptr<SampledImage> Create
    (
      const VkFormat& format, 
      const VkImageType& type,
      const VkExtent3D& extent, 
      const std::shared_ptr<Sampler>& sampler = nullptr,
      const SamplerCreateInfo& sampler_create_info = {}, 
      const VkComponentMapping& view_component_mapping = YK_DEFAULT_VIEW_COMPONENT_MAPPING,
      const uint32_t& layer_count = 1, 
      const uint32_t& mipmap_levels = 1, 
      bool copiable = false, 
      const VkImageCreateFlags& flags = 0,
      const VkSampleCountFlagBits& sample_count = VK_SAMPLE_COUNT_1_BIT
    );

  private:
    std::shared_ptr<Sampler> r_Sampler;
  };

}