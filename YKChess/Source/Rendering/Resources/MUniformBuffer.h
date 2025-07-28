#pragma once

#include <memory>

#include "Rendering/Resources/AbstractionLayers/MappableBuffer.h"

namespace yk
{

  class MUniformBuffer : public MappableBuffer
  {
  public:
    ~MUniformBuffer();

    static std::shared_ptr<MUniformBuffer> Create(const BufferCreateInfo& info);
    static std::shared_ptr<MUniformBuffer> Create(const VkDeviceSize& size, bool copyable = false);
  };

}