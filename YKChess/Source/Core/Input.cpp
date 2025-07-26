#include <vkrenpch.h>

#include <GLFW/glfw3.h>

#include "VulkanRenderer/Core/Application.h"
#include "VulkanRenderer/Core/Input.h"

namespace vkren
{
  bool Input::IsKeyPressed(KeyCode key)
  {
    GLFWwindow* window = Application::GetWindow().GetNative();
    auto state = glfwGetKey(window, static_cast<uint32_t>(key));
    return state == GLFW_PRESS;
  }

  bool Input::IsMouseButtonPressed(MouseCode button)
  {
    GLFWwindow* window = Application::GetWindow().GetNative();
    auto state = glfwGetMouseButton(window, static_cast<uint32_t>(button));
    return state == GLFW_PRESS;
  }

  glm::vec2 Input::GetMousePos()
  {
    GLFWwindow* window = Application::GetWindow().GetNative();
    double x_pos, y_pos;
    glfwGetCursorPos(window, &x_pos, &y_pos);
    return glm::vec2(x_pos, y_pos);
  }

}