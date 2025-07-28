#pragma once

#include <glm/glm.hpp>

#include "Core/MouseCodes.h"
#include "Core/KeyCodes.h"

namespace yk
{
  class Input
  {
  public:
    static bool IsKeyPressed(KeyCode key);
    static bool IsMouseButtonPressed(MouseCode button);
    static glm::vec2 GetMousePosition();

  private:
    Input() = delete;
    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;
    Input(Input&&) = delete;
    Input& operator=(Input&&) = delete;
  };
}
