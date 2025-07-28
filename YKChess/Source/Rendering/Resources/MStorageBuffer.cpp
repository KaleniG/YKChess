#include "Rendering/Resources/MStorageBuffer.h"
#include "Rendering/Renderer.h"
#include "Rendering/Utils.h"

namespace yk
{

  void* MStorageBuffer::GetMappedData(const VkDeviceSize& size, const VkDeviceSize& offset)
  {
    YK_ASSERT(m_Mapped, "Vulkan/System:  This buffer's memory isn't mapped");

    VkMappedMemoryRange memoryRange = {};
    memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    memoryRange.memory = m_Memory;
    memoryRange.offset = offset;
    memoryRange.size = size ? size : VK_WHOLE_SIZE;

    VkResult result = vkFlushMappedMemoryRanges(Renderer::GetDevice()->GetLogical(), 1, &memoryRange);
    YK_ASSERT(result == VK_SUCCESS, "Vulkan: Failed to flush mapped memory. {}", Utils::VkResultToString(result));

    return m_Data;
  }

  MStorageBuffer MStorageBuffer::Create(const BufferCreateInfo& info)
  {
    return MStorageBuffer::Create(info.Size, info.Copyable);
  }

  MStorageBuffer MStorageBuffer::Create(const VkDeviceSize& size, bool copyable)
  {
    MStorageBuffer buffer;
    buffer.CreateMappableBuffer((copyable ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT : 0) | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, size);
    return buffer;
  }

}