#pragma once

#include "Rendering/Resources/AbstractionLayers/Buffer.h"

namespace yk
{

  class TransferBuffer : public Buffer
  {
  public:
    static TransferBuffer Create(Image& src_image, bool copy = true);
    static TransferBuffer Create(Buffer& src_buffer, bool copy = true);
    static TransferBuffer Create(const VkDeviceSize& size);
  };

}