#pragma once

#include <string>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Core/EventManager.h"
#include "Core/Timestep.h"
#include "Core/Input.h"

namespace yk
{
  class WindowManager
  {
  public:
    static void InitWindow(uint32_t width, uint32_t height, const std::string& title);
    static void Shutdown();

    static void UpdateWindow(Timestep timestep);

    static const std::string& GetWindowTitle();
    static void SetWindowTitle(const std::string& title);
    static glm::uvec2 GetWindowSize();
    static void SetWindowSize(glm::uvec2 size);

    static bool IsInitialized();

  private:
    static GLFWwindow* GetWindow();

    static WindowManager& Get() { static WindowManager instance; return instance; }
    WindowManager() = default;
    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;
    WindowManager(WindowManager&&) = delete;
    WindowManager& operator=(WindowManager&&) = delete;

  private:
    GLFWwindow* s_Window = nullptr;
    std::string s_Title;
    bool s_Initialized = false;

  private:
    friend class EventManager;
    friend class Input;
  };

}