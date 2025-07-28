#include "Rendering/Resources/AbstractionLayers/Image.h"
#include "Rendering/Resources/TransferBuffer.h"
#include "Rendering/Renderer.h"
#include "Rendering/Utils.h"

namespace yk
{

  TransferBuffer TransferBuffer::Create(Image& src_image, bool copy)
  {
    TransferBuffer buffer = TransferBuffer::Create(src_image.GetSize());

    if (copy)
      src_image.CopyToBuffer(static_cast<Buffer&>(buffer));

    return buffer;
  }

  TransferBuffer TransferBuffer::Create(Buffer& src_buffer, bool copy)
  {
    TransferBuffer& src_buffer_temp = static_cast<TransferBuffer&>(src_buffer);
    TransferBuffer buffer = TransferBuffer::Create(src_buffer_temp.m_Size);

    if (copy)
      src_buffer.CopyToBuffer(static_cast<Buffer&>(buffer));

    return buffer;
  }

  TransferBuffer TransferBuffer::Create(const VkDeviceSize& size)
  {
    TransferBuffer buffer;
    buffer.CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, size);
    return buffer;
  }

}