#pragma once

#include "Rendering/Resources/AbstractionLayers/Buffer.h"

namespace yk
{

  struct UniformTexelBufferCreateInfo
  {
    BufferCreateInfo Buffer = {};
    BufferViewCreateInfo View = {};
  };

  class UniformTexelBuffer : public Buffer
  {
  public:
    ~UniformTexelBuffer();

    const VkBufferView& GetView() const { return m_View; }

    static UniformTexelBuffer Create(const UniformTexelBufferCreateInfo& info);
    static UniformTexelBuffer Create
    (
      const VkDeviceSize& size, 
      bool copiable = false, 
      const VkFormat& format = VK_FORMAT_UNDEFINED, 
      const VkDeviceSize& range = 0, 
      const VkDeviceSize& offset = 0
    );

  private:
    VkBufferView m_View = VK_NULL_HANDLE;
  };

}