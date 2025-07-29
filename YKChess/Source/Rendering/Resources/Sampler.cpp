#include "Rendering/Resources/Sampler.h"
#include "Rendering/Renderer.h"
#include "Rendering/Utils.h"

namespace yk
{

  Sampler::~Sampler()
  {
    vkDestroySampler(Renderer::GetDevice()->GetLogical(), m_Sampler, VK_NULL_HANDLE);
  }

  std::shared_ptr<Sampler> Sampler::Create(const SamplerCreateInfo& info)
  {
    YK_ASSERT(info.ImageFormat != VK_FORMAT_UNDEFINED, "Vulkan/System:  Specify a format");
    std::shared_ptr<Sampler> sampler = std::make_shared<Sampler>();

    VkSamplerCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.addressModeU = info.AddressModeU;
    samplerCreateInfo.addressModeV = info.AddressModeV;
    samplerCreateInfo.addressModeW = info.AddressModeW;
    samplerCreateInfo.compareEnable = info.EnableDepthCompare;
    samplerCreateInfo.compareOp = info.DepthCompareOperation;
    samplerCreateInfo.borderColor = info.BorderColor;
    samplerCreateInfo.unnormalizedCoordinates = info.UnnormalizedCoordinates;

    samplerCreateInfo.magFilter = info.MagFilter;
    samplerCreateInfo.minFilter = info.MinFilter;
    samplerCreateInfo.mipmapMode = info.MipmapFilterMode;

    sampler->m_LinearFiltering = (info.MagFilter == VK_FILTER_LINEAR || info.MinFilter == VK_FILTER_LINEAR || info.MipmapFilterMode == VK_SAMPLER_MIPMAP_MODE_LINEAR);

    samplerCreateInfo.anisotropyEnable = info.EnableAnistropicFiltering;
    if (info.EnableAnistropicFiltering)
    {
      YK_ASSERT(info.MaxAnistropyAngle <= Renderer::GetDevice()->GetPhysicalDeviceProperties().limits.maxSamplerAnisotropy && info.MaxAnistropyAngle >= 1.0f, "Vulkan/System:  Inavalid/Unsupported 'MaxAnistropyAngle' specified");
      samplerCreateInfo.maxAnisotropy = info.MaxAnistropyAngle;
    }

    samplerCreateInfo.minLod = info.MinLOD;
    samplerCreateInfo.maxLod = info.MaxLOD;
    YK_ASSERT(info.MinLOD > 0.0f && info.MaxLOD > 0.0f && info.MaxLOD <= info.MinLOD, "Vulkan/System:  Invalid 'MaxLOD/MinLOD' value for the image");

    VkResult result = vkCreateSampler(Renderer::GetDevice()->GetLogical(), &samplerCreateInfo, VK_NULL_HANDLE, &sampler->m_Sampler);
    YK_ASSERT(result == VK_SUCCESS, "Vulkan: Failed to create the image sampler. {}", Utils::VkResultToString(result));

    return sampler;
  }

}