#include <stb_image.h>
#include <YKLib.h>

#include "Core/WindowManager.h"

namespace yk
{
  void WindowManager::InitWindow(uint32_t width, uint32_t height, const std::string& title)
  {
		YK_ASSERT(!WindowManager::IsInitialized(), "A window is already initialized");

		WindowManager::Get().s_Title = title;
		
    int32_t success = glfwInit();
    YK_ASSERT(success, "Could not intialize GLFW");

    success = glfwVulkanSupported();
    YK_ASSERT(success, "Vulkan is not supported");

		glfwSetErrorCallback([](int32_t error_code, const char* description) 
			{
				YK_ERROR("GLFW error '{}': {}", error_code, description);
			});

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    WindowManager::Get().s_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    YK_ASSERT(WindowManager::Get().s_Window, "Failed to create a GLFW window");

		WindowManager::Get().s_Initialized = true;
  }

  void WindowManager::Shutdown()
  {
    glfwDestroyWindow(WindowManager::GetWindow());
    glfwTerminate();
    WindowManager::Get().s_Initialized = false;
  }

  void WindowManager::UpdateWindow(Timestep timestep)
  {
		YK_ASSERT(WindowManager::IsInitialized(), "A window hasn't been initialized");
		glfwPollEvents();
  }

  void WindowManager::WaitEvents()
  {
    glfwWaitEvents();
  }

  const std::string& WindowManager::GetWindowTitle()
  {
		YK_ASSERT(WindowManager::IsInitialized(), "A window hasn't been initialized");
		return WindowManager::Get().s_Title;
  }

  void WindowManager::SetWindowTitle(const std::string& title)
  {
		glfwSetWindowTitle(WindowManager::GetWindow(), title.c_str());
		WindowManager::Get().s_Title = title;
  }

  glm::uvec2 WindowManager::GetWindowSize()
  {
    int32_t width, height;
    glfwGetWindowSize(WindowManager::GetWindow(), &width, &height);
    return { width, height };
  }

  void WindowManager::SetWindowSize(glm::uvec2 size)
  {
    glfwSetWindowSize(WindowManager::GetWindow(), size.x, size.y);
  }

  glm::uvec2 WindowManager::GetFramebufferSize()
  {
    int32_t width, height;
    glfwGetFramebufferSize(WindowManager::GetWindow(), &width, &height);
    return { width, height };
  }

  void WindowManager::SetWindowIcon(const std::vector<std::filesystem::path>& icons)
  {
    std::vector<GLFWimage> glfwIcons;
    glfwIcons.reserve(icons.size());

    std::vector<stbi_uc*> pixelBuffers;
    pixelBuffers.reserve(icons.size());

    for (const auto& filepath : icons)
    {
      YK_ASSERT(filepath.extension() == ".png", "System: Only '.png' image format supported for now");

      int32_t width, height, channelsCount;
      stbi_uc* pixels = stbi_load(filepath.string().c_str(), &width, &height, &channelsCount, STBI_rgb_alpha);
      YK_ASSERT(pixels, "Failed to load the image");

      pixelBuffers.push_back(pixels);

      GLFWimage icon;
      icon.width = width;
      icon.height = height;
      icon.pixels = pixels;

      glfwIcons.push_back(icon);
    }

    glfwSetWindowIcon(WindowManager::GetWindow(), static_cast<int32_t>(glfwIcons.size()), glfwIcons.data());

    for (stbi_uc* pixels : pixelBuffers)
      stbi_image_free(pixels);
  }

  bool WindowManager::IsInitialized()
  {
    return WindowManager::Get().s_Initialized;
  }

  GLFWwindow* WindowManager::GetWindow()
  {
		YK_ASSERT(WindowManager::IsInitialized(), "A window hasn't been initialized");
		return WindowManager::Get().s_Window;
  }
}