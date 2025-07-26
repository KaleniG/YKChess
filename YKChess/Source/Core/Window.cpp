#include <vkrenpch.h>

#include "VulkanRenderer/Core/Window.h"
#include "VulkanRenderer/Events/ApplicationEvent.h"
#include "VulkanRenderer/Events/KeyEvent.h"
#include "VulkanRenderer/Events/MouseEvent.h"
#include "VulkanRenderer/Renderer/Renderer.h"

namespace vkren
{
  static bool s_GLFWInitialized = false;

  static void GLFWErrorCallback(int error, const char* description)
  {
    CORE_ERROR("[GLFW] Error ({0}): {1}", error, description);
  }

  Window::Window(const WindowConfig& config)
  {
    m_WindowData.Title = config.Title;
    m_WindowData.Width = config.Width;
    m_WindowData.Height = config.Height;

    if (!s_GLFWInitialized)
    {
      int success = glfwInit();
      CORE_ASSERT(success, "[GLFW] Could not intialize GLFW");

			success = glfwVulkanSupported();
			CORE_ASSERT(success, "[GLFW] Vulkan is not supported");

      #if defined(STATUS_DEBUG) || defined(STATUS_RELEASE)
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
      #endif

      glfwSetErrorCallback(GLFWErrorCallback);
      s_GLFWInitialized = true;
    }

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_Window = glfwCreateWindow(m_WindowData.Width, m_WindowData.Height, m_WindowData.Title.c_str(), nullptr, nullptr);

		CORE_ASSERT(m_Window != nullptr, "[GLFW] Failed to create a GLFW window");

    glfwSetWindowUserPointer(m_Window, &m_WindowData);

    glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int32_t width, int32_t height)
      {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.Width = width;
        data.Height = height;

        WindowResizeEvent event(width, height);
        data.EventCallback(event);
      });

    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
      {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        WindowCloseEvent event;
        data.EventCallback(event);
      });

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int32_t button, int32_t action, int32_t mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double x_offset, double y_offset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)x_offset, (float)y_offset);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double x_pos, double y_pos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)x_pos, (float)y_pos);
				data.EventCallback(event);
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, uint32_t keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keycode);
				data.EventCallback(event);
			});

  }

  Window::~Window()
  {
		glfwDestroyWindow(m_Window);
		glfwTerminate();
  }

  void Window::OnUpdate(Timestep timestep, ImDrawData* imgui_draw_data)
  {
		glfwPollEvents();
		Renderer::DrawFrame(timestep, imgui_draw_data);
  }

	void Window::SetCursorEnabled(bool enabled)
	{
		if (m_MouseEnabled == enabled)
			return;
		glfwSetInputMode(m_Window, GLFW_CURSOR, enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED | GLFW_CURSOR_HIDDEN);
		m_MouseEnabled = enabled;
	}

}