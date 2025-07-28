#pragma once

#include "Rendering/Resources/AbstractionLayers/Buffer.h"

namespace yk
{

  class StorageBuffer : public Buffer
  {
  public:
    static StorageBuffer Create(const BufferCreateInfo& info);
    static StorageBuffer Create(const VkDeviceSize& size, bool copyable = false);
  };

}
