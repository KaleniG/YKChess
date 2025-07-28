#pragma once

#include "Rendering/Resources/AbstractionLayers/Buffer.h"

namespace yk
{
  enum class FlushType
  {
    Invalidate, Flush
  };

  class MappableBuffer : public Buffer
  {
  public:
    void Map(const VkDeviceSize& size = VK_WHOLE_SIZE, const VkDeviceSize& offset = 0);
    void Update(void* data, const VkDeviceSize& size = 0, const VkDeviceSize& offset = 0, bool flush = true);
    void Unmap();

  protected:
    void CreateMappableBuffer(const VkBufferUsageFlags& usage, const VkDeviceSize& size);

  protected:
    void* m_Data = nullptr;
    bool m_Mapped = false;
  };
}