#pragma once

#include <vulkan/vulkan.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

namespace yk
{

  struct alignas(16) UniformBufferObject
  {
    glm::mat4 Projection;
    glm::vec2 MousePosition;
  };

  struct alignas(16) Vertex
  {
    glm::vec2 Position;
    glm::vec2 TextureCoord;

    bool operator==(const Vertex& other) const { return Position == other.Position && TextureCoord == other.TextureCoord; }
  };

}

namespace std
{
  template<> struct hash<yk::Vertex>
  {
    size_t operator()(yk::Vertex const& vertex) const
    {
      size_t seed = 0;
      hash_combine(seed, vertex.Position);
      hash_combine(seed, vertex.TextureCoord);
      return seed;
    }

  private:
    template<typename T>
    void hash_combine(size_t& seed, T const& v) const
    {
      seed ^= hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
  };
}