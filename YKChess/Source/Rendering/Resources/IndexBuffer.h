#pragma once

#include <memory>
#include <vector>

#include "Rendering/Resources/AbstractionLayers/Buffer.h"

namespace yk
{
  class IndexBuffer : public Buffer
  {
  public:
    uint32_t GetIndexCount() const { return m_IndexCount; }

    static std::shared_ptr<IndexBuffer> Create(const std::vector<uint32_t>& indices);
    static std::shared_ptr<IndexBuffer> Create(VkDeviceSize size, bool copyable = false);

  private:
    uint32_t m_IndexCount;
  };
}