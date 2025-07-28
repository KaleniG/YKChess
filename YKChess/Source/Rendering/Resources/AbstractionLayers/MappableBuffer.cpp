#include <YKLib.h>

#include "Rendering/Resources/AbstractionLayers/MappableBuffer.h"
#include "Rendering/Renderer.h"
#include "Rendering/Utils.h"

namespace yk
{
  void MappableBuffer::Map(const VkDeviceSize& size, const VkDeviceSize& offset)
  {
    YK_ASSERT(!m_Mapped, "Vulkan/System: memory already mapped");
    VkResult result = vkMapMemory(Renderer::GetDevice()->GetLogical(), m_Memory, offset, size, 0, &m_Data);
    YK_ASSERT(result == VK_SUCCESS, "Vulkan: failed to map the memory. {}", Utils::VkResultToString(result));

    m_Mapped = true;
  }

  void MappableBuffer::Update(void* data, const VkDeviceSize& size, const VkDeviceSize& offset, bool flush)
  {
    YK_ASSERT(m_Mapped, "Vulkan/System: this buffer's memory isn't mapped");
    std::memcpy(m_Data, data, static_cast<size_t>(size ? size : m_Size));

    if (flush)
    {
      VkMappedMemoryRange memoryRange = {};
      memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
      memoryRange.memory = m_Memory;
      memoryRange.offset = offset;
      memoryRange.size = size ? size : VK_WHOLE_SIZE;

      VkResult result = vkInvalidateMappedMemoryRanges(Renderer::GetDevice()->GetLogical(), 1, &memoryRange);
      YK_ASSERT(result == VK_SUCCESS, "Vulkan: failed to invalidate mapped memory. {}", Utils::VkResultToString(result));
    }
  }

  void MappableBuffer::Unmap()
  {
    if (!m_Mapped)
    {
      YK_WARN("Vulkan/System: memory already unmapped");
      return;
    }
    vkUnmapMemory(Renderer::GetDevice()->GetLogical(), m_Memory);
    m_Mapped = false;
  }

  void MappableBuffer::CreateMappableBuffer(const VkBufferUsageFlags& usage, const VkDeviceSize& size)
  {
    Buffer::CreateBuffer(usage, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, size);
  }
}