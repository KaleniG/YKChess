#include <GLFW/glfw3.h>

#include "Core/WindowManager.h"
#include "Core/EventManager.h"

namespace yk
{
  std::vector<EventManager*> EventManager::s_GlobalEventManagers;

  EventManager::EventManager()
  {
    s_GlobalEventManagers.push_back(this);

    EventManager::UpdateCallbacks();
  }

  EventManager::~EventManager()
  {
    if (!WindowManager::IsInitialized())
    {
      s_GlobalEventManagers.clear();
      return;
    }

    for (int i = 0; i < s_GlobalEventManagers.size(); i++)
    {
      if (s_GlobalEventManagers[i] == this)
      {
        s_GlobalEventManagers.erase(s_GlobalEventManagers.begin() + i);
        EventManager::UpdateCallbacks();
        return;
      }
    }
  }

  void EventManager::UpdateCallbacks()
  {
    GLFWwindow* window = WindowManager::GetWindow();

    glfwSetWindowUserPointer(window, this);

    glfwSetWindowCloseCallback(window, [](GLFWwindow* window)
      {
        EventManager* eventManager = static_cast<EventManager*>(glfwGetWindowUserPointer(window));
        std::vector<EventManager*>& eventManagers = eventManager->s_GlobalEventManagers;

        for (EventManager* manager : eventManagers)
        {
          manager->OnWindowClose();
        }
      });

    glfwSetWindowIconifyCallback(window, [](GLFWwindow* window, int32_t iconified)
      {
        EventManager* eventManager = static_cast<EventManager*>(glfwGetWindowUserPointer(window));
        std::vector<EventManager*>& eventManagers = eventManager->s_GlobalEventManagers;

        for (EventManager* manager : eventManagers)
        {
          if (iconified)
            manager->OnWindowIconify(true);
          else
            manager->OnWindowIconify(false);
        }
      });

    glfwSetWindowMaximizeCallback(window, [](GLFWwindow* window, int32_t maximized)
      {
        EventManager* eventManager = static_cast<EventManager*>(glfwGetWindowUserPointer(window));
        std::vector<EventManager*>& eventManagers = eventManager->s_GlobalEventManagers;

        for (EventManager* manager : eventManagers)
        {
          if (maximized)
            manager->OnWindowMaximize(true);
          else
            manager->OnWindowMaximize(false);
        }
      });

    glfwSetWindowFocusCallback(window, [](GLFWwindow* window, int32_t focused)
      {
        EventManager* eventManager = static_cast<EventManager*>(glfwGetWindowUserPointer(window));
        std::vector<EventManager*>& eventManagers = eventManager->s_GlobalEventManagers;

        for (EventManager* manager : eventManagers)
        {
          if (focused)
            manager->OnWindowFocus(true);
          else
            manager->OnWindowFocus(false);
        }
      });

    glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int32_t width, int32_t height)
      {
        EventManager* eventManager = static_cast<EventManager*>(glfwGetWindowUserPointer(window));
        std::vector<EventManager*>& eventManagers = eventManager->s_GlobalEventManagers;

        for (EventManager* manager : eventManagers)
        {
          manager->OnWindowResize(width, height);
        }
      });

    glfwSetWindowPosCallback(window, [](GLFWwindow* window, int32_t xpos, int32_t ypos)
      {
        EventManager* eventManager = static_cast<EventManager*>(glfwGetWindowUserPointer(window));
        std::vector<EventManager*>& eventManagers = eventManager->s_GlobalEventManagers;

        for (EventManager* manager : eventManagers)
        {
          manager->OnWindowMove(xpos, ypos);
        }
      });

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int32_t width, int32_t height)
      {
        EventManager* eventManager = static_cast<EventManager*>(glfwGetWindowUserPointer(window));
        std::vector<EventManager*>& eventManagers = eventManager->s_GlobalEventManagers;

        for (EventManager* manager : eventManagers)
        {
          manager->OnFramebufferResize(width, height);
        }
      });

    glfwSetKeyCallback(window, [](GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
      {
        EventManager* eventManager = static_cast<EventManager*>(glfwGetWindowUserPointer(window));
        std::vector<EventManager*>& eventManagers = eventManager->s_GlobalEventManagers;

        switch (action)
        {
        case GLFW_PRESS:
        {
          for (EventManager* manager : eventManagers)
          {
            manager->OnKeyPress(KeyCode(key), false);
          }
          break;
        }
        case GLFW_RELEASE:
        {
          for (EventManager* manager : eventManagers)
          {
            manager->OnKeyRelease(KeyCode(key));
          }
          break;
        }
        case GLFW_REPEAT:
        {
          for (EventManager* manager : eventManagers)
          {
            manager->OnKeyPress(KeyCode(key), true);
          }
          break;
        }
        }
      });

    glfwSetCharCallback(window, [](GLFWwindow* window, uint32_t codepoint)
      {
        EventManager* eventManager = static_cast<EventManager*>(glfwGetWindowUserPointer(window));
        std::vector<EventManager*>& eventManagers = eventManager->s_GlobalEventManagers;

        for (EventManager* manager : eventManagers)
        {
          manager->OnCharacterType(static_cast<char32_t>(codepoint));
        }
      });

    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int32_t button, int32_t action, int32_t mods)
      {
        EventManager* eventManager = static_cast<EventManager*>(glfwGetWindowUserPointer(window));
        std::vector<EventManager*>& eventManagers = eventManager->s_GlobalEventManagers;

        switch (action)
        {
        case GLFW_PRESS:
        {
          for (EventManager* manager : eventManagers)
          {
            manager->OnMouseButtonPress(MouseCode(button));
          }
          break;
        }
        case GLFW_RELEASE:
        {
          for (EventManager* manager : eventManagers)
          {
            manager->OnMouseButtonRelease(MouseCode(button));
          }
          break;
        }
        }
      });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
      {
        EventManager* eventManager = static_cast<EventManager*>(glfwGetWindowUserPointer(window));
        std::vector<EventManager*>& eventManagers = eventManager->s_GlobalEventManagers;

        for (EventManager* manager : eventManagers)
        {
          manager->OnMouseScroll(xoffset, yoffset);
        }
      });

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
      {
        EventManager* eventManager = static_cast<EventManager*>(glfwGetWindowUserPointer(window));
        std::vector<EventManager*>& eventManagers = eventManager->s_GlobalEventManagers;

        for (EventManager* manager : eventManagers)
        {
          manager->OnMouseMove(xpos, ypos);
        }
      });
  }
}