#include "Rendering/Resources/MUniformBuffer.h"
#include "Rendering/Renderer.h"
#include "Rendering/Utils.h"

namespace yk
{

  MUniformBuffer::~MUniformBuffer()
  {
    MUniformBuffer::Unmap();
  }

  std::shared_ptr<MUniformBuffer> MUniformBuffer::Create(const BufferCreateInfo& info)
  {
    return MUniformBuffer::Create(info.Size, info.Copyable);
  }

  std::shared_ptr<MUniformBuffer> MUniformBuffer::Create(const VkDeviceSize& size, bool copyable)
  {
    std::shared_ptr<MUniformBuffer> buffer = std::make_shared<MUniformBuffer>();

    buffer->CreateMappableBuffer((copyable ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT : 0) | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, size);
    buffer->Map();

    return buffer;
  }

}