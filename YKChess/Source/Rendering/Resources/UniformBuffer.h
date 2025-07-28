#pragma once

#include <memory>

#include "Rendering/Resources/AbstractionLayers/Buffer.h"

namespace yk
{

  class UniformBuffer : public Buffer
  {
  public:
    static std::shared_ptr<UniformBuffer> Create(const BufferCreateInfo& info);
    static std::shared_ptr<UniformBuffer> Create(const VkDeviceSize& size, bool copyable = false);
  };

}