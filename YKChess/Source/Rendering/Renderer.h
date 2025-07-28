#pragma once

#include <memory>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include "Rendering/DebugOverlayManager.h"
#include "Rendering/ResourceManager.h"
#include "Rendering/Device.h"

namespace yk
{
  class ImageResource;
  class FontResource;

  class Renderer
  {
  public:
    static void Init();
    static void Shutdown();

    static void DrawImage(glm::vec2 position, glm::vec2 size, std::shared_ptr<ImageResource> image);
    static void DrawText(const std::string& text, glm::vec2 size, glm::vec2 position, glm::vec4 color, std::shared_ptr<FontResource> font);

    static void RenderBatch();

    static bool IsInitialized();

    static std::shared_ptr<Device> GetDevice();

  private:
    static Renderer& Get() { static Renderer instance; return instance; }
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

  private:
    bool s_Initialized = false;

    std::shared_ptr<Device> s_Device;

  private:
    friend class DebugOverlayManager;
    friend class ResourceManager;
  };
}