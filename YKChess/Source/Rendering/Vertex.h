#pragma once

#include <vulkan/vulkan.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

namespace yk
{

  struct alignas(16) ShaderUBO
  {
    glm::mat4 Projection;
    glm::vec2 MousePosition;
  };

  struct alignas(16) Vertex
  {
    glm::vec3 Position;
    glm::vec2 UV;
    uint32_t ID;

    bool operator==(const Vertex& other) const { return Position == other.Position && UV == other.UV && ID == other.ID; }
  };

}