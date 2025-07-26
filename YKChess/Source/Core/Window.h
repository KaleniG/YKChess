#pragma once

#include <imgui.h>
#include <GLFW/glfw3.h>

#include "VulkanRenderer/Core/Timestep.h"
#include "VulkanRenderer/Events/Event.h"

namespace vkren
{

  struct WindowConfig
  {
    std::string Title;
    uint32_t Width;
    uint32_t Height;
  };

  class Window
  {
  private:
    using EventCallbackFn = std::function<void(Event&)>;

  public:
    Window() = default;
    Window(const WindowConfig& config);
    ~Window();

    void OnUpdate(Timestep timestep, ImDrawData* imgui_draw_data = nullptr);

    const std::string& GetName() const { return m_WindowData.Title; }
    uint32_t GetWidth() const { return m_WindowData.Width; }
    uint32_t GetHeight() const { return m_WindowData.Height; }

    void SetEventCallback(const EventCallbackFn& callback) { m_WindowData.EventCallback = callback; }
    void SetCursorEnabled(bool enabled);

    GLFWwindow* GetNative() { return m_Window; }

  private:
    GLFWwindow* m_Window;
    
    struct WindowData
    {
      std::string Title;
      uint32_t Width;
      uint32_t Height;
      EventCallbackFn EventCallback;
    };

    bool m_MouseEnabled = true;
    WindowData m_WindowData;
  };

}