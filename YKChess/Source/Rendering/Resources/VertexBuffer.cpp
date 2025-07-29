#include "Rendering/Resources/VertexBuffer.h"
#include "Rendering/Resources/StagingBuffer.h"

namespace yk
{

  std::shared_ptr<VertexBuffer> VertexBuffer::Create(VkDeviceSize size, bool copyable)
  {
    std::shared_ptr<VertexBuffer> buffer = std::make_shared<VertexBuffer>();
    buffer->CreateBuffer((copyable ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT : 0) | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, size);
    return buffer;
  }

}