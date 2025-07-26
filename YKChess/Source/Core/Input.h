#pragma once

#include <glm/glm.hpp>

#include "VulkanRenderer/Core/MouseCodes.h"
#include "VulkanRenderer/Core/KeyCodes.h"
#include "VulkanRenderer/Core/Base.h"

namespace vkren
{

  class Input
  {
  public:
    static bool IsKeyPressed(KeyCode key);
    static bool IsMouseButtonPressed(MouseCode button);
    static glm::vec2 GetMousePos();
    Input& operator=(const Input&) = delete;
  };

}
