#pragma once

#include "Rendering/Resources/AbstractionLayers/MappableBuffer.h"

namespace yk
{

  class MStorageBuffer : public MappableBuffer
  {
  public:
    void* GetMappedData(const VkDeviceSize& size = 0, const VkDeviceSize& offset = 0);

    static MStorageBuffer Create(const BufferCreateInfo& info);
    static MStorageBuffer Create(const VkDeviceSize& size, bool copyable = false);
  };

}
