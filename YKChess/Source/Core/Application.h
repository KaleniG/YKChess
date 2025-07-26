#pragma once

#include "VulkanRenderer/Core/LayerStack.h"
#include "VulkanRenderer/Core/Timestep.h"
#include "VulkanRenderer/Core/Window.h"
#include "VulkanRenderer/Events/ApplicationEvent.h"
#include "VulkanRenderer/ImGui/ImGuiLayer.h"
#include "VulkanRenderer/Renderer/Renderer.h"

namespace vkren
{

  struct ApplicationConfig
  {
    std::string Name;

    RendererConfig Renderer;
    WindowConfig Window;
  };

  class Application
  {
  public:
    Application(const ApplicationConfig& config);
    ~Application();

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);

    void OnEvent(Event& e);

    void Run();

    static Window& GetWindow() { return *Get().m_Window.get(); }
    static const std::string& GetName() { return Get().m_Config.Name; }

  private:
    static Application& Get() { return *s_ApplicationInstance; }

    Timestep CalculateTimestep();

    bool OnWindowClose(WindowCloseEvent& e);
    bool OnWindowResize(WindowResizeEvent& e);

  private:
    ApplicationConfig m_Config;

    Scope<Window> m_Window;
    ImGuiLayer* m_ImGuiLayer;
    LayerStack m_LayerStack;

    bool m_IsRunning = true;
    bool m_IsMinimized = false;

  private:
    static Application* s_ApplicationInstance;
  };

  Application* CreateApplication();

}