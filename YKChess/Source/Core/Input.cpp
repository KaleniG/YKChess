#include <GLFW/glfw3.h>

#include "Core/Input.h"
#include "Core/WindowManager.h"

namespace yk
{
  bool Input::IsKeyPressed(KeyCode key)
  {
    GLFWwindow* window = WindowManager::GetWindow();
    int32_t state = glfwGetKey(window, static_cast<uint32_t>(key));
    return state == GLFW_PRESS;
  }

  bool Input::IsMouseButtonPressed(MouseCode button)
  {
    GLFWwindow* window = WindowManager::GetWindow();
    int32_t state = glfwGetMouseButton(window, static_cast<uint32_t>(button));
    return state == GLFW_PRESS;
  }

  glm::vec2 Input::GetMousePosition()
  {
    GLFWwindow* window = WindowManager::GetWindow();
    double x_pos, y_pos;
    glfwGetCursorPos(window, &x_pos, &y_pos);
    return { x_pos, y_pos };
  }
}