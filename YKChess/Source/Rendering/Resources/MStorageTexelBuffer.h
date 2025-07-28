#pragma once

#include "Rendering/Resources/AbstractionLayers/MappableBuffer.h"

namespace yk
{

  struct MStorageTexelBufferCreateInfo
  {
    BufferCreateInfo Buffer = {};
    BufferViewCreateInfo View = {};
  };

  class MStorageTexelBuffer : public MappableBuffer
  {
  public:
    ~MStorageTexelBuffer();

    const VkBufferView& GetView() const { return m_View; }

    void* GetMappedData(const VkDeviceSize& size = 0, const VkDeviceSize& offset = 0);

    static MStorageTexelBuffer Create(const MStorageTexelBufferCreateInfo& info);
    static MStorageTexelBuffer Create
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