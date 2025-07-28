#include "Rendering/Resources/UniformBuffer.h"

namespace yk
{

  std::shared_ptr<UniformBuffer> UniformBuffer::Create(const BufferCreateInfo& info)
  {
    return UniformBuffer::Create(info.Size, info.Copyable);
  }

  std::shared_ptr<UniformBuffer> UniformBuffer::Create(const VkDeviceSize& size, bool copyable)
  {
    std::shared_ptr<UniformBuffer> buffer = std::shared_ptr<UniformBuffer>();

    buffer->CreateBuffer((copyable ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT : 0) | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, size);

    return buffer;
  }

}