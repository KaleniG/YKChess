#include "Rendering/Resources/StorageBuffer.h"

namespace yk
{

  StorageBuffer StorageBuffer::Create(const BufferCreateInfo& info)
  {
    return StorageBuffer::Create(info.Size, info.Copyable);
  }

  StorageBuffer StorageBuffer::Create(const VkDeviceSize& size, bool copyable)
  {
    StorageBuffer buffer;
    buffer.CreateBuffer((copyable ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT : 0) | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, size);
    return buffer;
  }

}