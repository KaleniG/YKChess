#pragma once

#include <memory>
#include <vector>

#include "Rendering/Resources/AbstractionLayers/Buffer.h"
#include "Rendering/Resources/StagingBuffer.h"

namespace yk
{

  struct Vertex;
  struct TerrainVertex;
  class VertexBuffer : public Buffer
  {
  public:
    template<typename T>
    static std::shared_ptr<VertexBuffer> Create(const std::vector<T>& vertices);
    static std::shared_ptr<VertexBuffer> Create(VkDeviceSize size, bool copyable = false);
  };

}

#include "Rendering/Resources/VertexBuffer.inl"