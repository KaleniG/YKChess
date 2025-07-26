#include <vkrenpch.h>

#include "VulkanRenderer/Core/Application.h"

namespace vkren
{

  Application* Application::s_ApplicationInstance = nullptr;

  Application::Application(const ApplicationConfig& config)
    : m_Config(config)
  {
    CORE_ASSERT(!s_ApplicationInstance, "Application already exists!");
    s_ApplicationInstance = this;

    m_Window = CreateScope<Window>(m_Config.Window);
    m_Window->SetEventCallback(VKREN_BIND_EVENT_FN(Application::OnEvent));

    RendererConfig rendererConfig;
    rendererConfig.Device.MaxFramesInFlight = 2;

    Renderer::Init(config.Renderer);

    m_ImGuiLayer = new ImGuiLayer();
    Application::PushOverlay(m_ImGuiLayer);
  }

  Application::~Application()
  {
    Renderer::Shutdown();
  }

  void Application::PushLayer(Layer* layer)
  {
    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
  }

  void Application::PushOverlay(Layer* overlay)
  {
    m_LayerStack.PushOverlay(overlay);
    overlay->OnAttach();
  }

  void Application::OnEvent(Event& e)
  {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(VKREN_BIND_EVENT_FN(Application::OnWindowClose));
    dispatcher.Dispatch<WindowResizeEvent>(VKREN_BIND_EVENT_FN(Application::OnWindowResize));

    for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
    {
      if (e.Handled)
        break;
      (*it)->OnEvent(e);
    }
  }

  void Application::Run()
  {
    while (m_IsRunning)
    {
      Timestep timestep = Application::CalculateTimestep();

      if (!m_IsMinimized)
      {
        for (Layer* layer : m_LayerStack)
          layer->OnUpdate(timestep);

        m_ImGuiLayer->Start();

        for (Layer* layer : m_LayerStack)
          layer->OnImGuiRender();
      }

      m_Window->OnUpdate(timestep, m_ImGuiLayer->Submit());
    }

    Renderer::OnExit();
    m_LayerStack.PopOverlay(m_ImGuiLayer);
  }

  Timestep Application::CalculateTimestep()
  {
    static auto lastTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float>(currentTime - lastTime).count();
    lastTime = currentTime;
    return Timestep(time);
  }

  bool Application::OnWindowClose(WindowCloseEvent& e)
  {
    m_IsRunning = false;
    return true;
  }

  bool Application::OnWindowResize(WindowResizeEvent& e)
  {
    if (e.GetWidth() == 0 || e.GetHeight() == 0)
    {
      m_IsMinimized = true;
      return false;
    }

    m_IsMinimized = false;

    return false;
  }

}