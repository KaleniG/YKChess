#pragma once

#include <memory>

#include <vulkan/vulkan.h>

namespace yk
{

  struct SamplerCreateInfo
  {
    VkFilter MagFilter = VK_FILTER_NEAREST;
    VkFilter MinFilter = VK_FILTER_NEAREST;
    VkSamplerMipmapMode MipmapFilterMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    VkSamplerAddressMode AddressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkSamplerAddressMode AddressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkSamplerAddressMode AddressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkBool32 EnableAnistropicFiltering = VK_FALSE;
    float MaxAnistropyAngle = 0;
    VkBool32 EnableDepthCompare = VK_TRUE;
    VkCompareOp DepthCompareOperation = VK_COMPARE_OP_LESS;
    float MinLOD = 1.0f;
    float MaxLOD = 1.0f;
    VkBorderColor BorderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    VkBool32 UnnormalizedCoordinates = VK_FALSE;

    VkFormat ImageFormat = VK_FORMAT_UNDEFINED;
    VkImageTiling ImageTiling = VK_IMAGE_TILING_OPTIMAL;
  };

  class Sampler
  {
  public:
    ~Sampler();

    const VkSampler& Get() const { return m_Sampler; }

    bool UsesLinearFiltering() const { return m_LinearFiltering; }

    static std::shared_ptr<Sampler> Create(const SamplerCreateInfo& info = {});

  private:
    VkSampler m_Sampler = VK_NULL_HANDLE;
    bool m_LinearFiltering = false;

  };

}