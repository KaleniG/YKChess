#pragma once

#include "Rendering/Resources/AbstractionLayers/Buffer.h"

namespace yk
{

  struct StorageTexelBufferCreateInfo
  {
    BufferCreateInfo Buffer = {};
    BufferViewCreateInfo View = {};
  };

  class StorageTexelBuffer : public Buffer
  {
  public:
    ~StorageTexelBuffer();

    const VkBufferView& GetView() const { return m_View; }

    static StorageTexelBuffer Create(const StorageTexelBufferCreateInfo& info);
    static StorageTexelBuffer Create
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