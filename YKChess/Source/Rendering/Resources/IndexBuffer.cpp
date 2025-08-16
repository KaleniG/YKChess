#include "Rendering/Resources/StagingBuffer.h"
#include "Rendering/Resources/IndexBuffer.h"

namespace yk
{

  std::shared_ptr<IndexBuffer> IndexBuffer::Create(const std::vector<uint32_t>& indices)
  {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    StagingBuffer stagingBuffer = StagingBuffer::Create(bufferSize);
    stagingBuffer.Map();
    stagingBuffer.UpdateData((void*)indices.data());
    stagingBuffer.Unmap();

    std::shared_ptr<IndexBuffer> buffer = IndexBuffer::Create(bufferSize);
    stagingBuffer.CopyToBuffer(*buffer.get());

    buffer->m_IndexCount = indices.size();

    return buffer;
  }

  std::shared_ptr<IndexBuffer> IndexBuffer::Create(VkDeviceSize size, bool copyable)
  {
    std::shared_ptr<IndexBuffer> buffer = std::make_shared<IndexBuffer>();
    buffer->CreateBuffer((copyable ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT : 0) | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, size);
    return buffer;
  }

}