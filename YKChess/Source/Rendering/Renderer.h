#pragma once

#include <memory>

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

#include "Rendering/DebugOverlayManager.h"

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

  private:
    static Renderer& Get() { static Renderer instance; return instance; }
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

  private:
    bool s_Initialized = false;

    vk::Instance s_VulkanInstance;
    vk::DebugUtilsMessengerEXT m_DebugMessenger;
    vk::SurfaceKHR m_Surface;
    vk::PhysicalDevice m_PhysicalDevice;
    vk::Device m_LogicalDevice;

    uint32_t m_GraphicsQueueFamilyIndex;
    vk::Queue m_GraphicsQueue;
    uint32_t m_PresentQueueFamilyIndex;
    vk::Queue m_PresentQueue;

    vk::Format m_DepthAttachmentFormat;

    vk::CommandPool m_CommandPool;

    vk::PhysicalDeviceMemoryProperties m_PhysicalDeviceMemoryProperties;
    vk::PhysicalDeviceProperties m_PhysicalDeviceProperties;

    vk::SampleCountFlagBits m_MaxSampleCount;

  private:
    friend class DebugOverlayManager;
  };
}