#pragma once

#include <imgui.h>
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace yk
{
  class Device
  {
  public:
    Device();
    ~Device();

    void WaitIdle();

    const VkInstance& GetVulkanInstance() const { return m_VulkanInstance; }
    const VkPhysicalDevice& GetPhysical() const { return m_PhysicalDevice; }
    const VkDevice& GetLogical() const { return m_LogicalDevice; }
    const VkSurfaceKHR& GetSurface() const { return m_Surface; }
    const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const { return m_PhysicalDeviceProperties; }

    uint32_t GetGraphicsQueueFamilyIndex() const { return m_GraphicsQueueFamilyIndex; }
    const VkQueue& GetGraphicsQueue() const { return m_GraphicsQueue; }
    uint32_t GetPresentQueueFamilyIndex() const { return m_PresentQueueFamilyIndex; }
    const VkQueue& GetPresentQueue() const { return m_PresentQueue; }

    uint32_t GetMinSwapchainImageCount();
    const VkExtent2D& GetSurfaceExtent() const;
    const VkFormat& GetDepthAttachmentFormat() const { return m_DepthAttachmentFormat; }
    const VkSampleCountFlagBits& GetMaxSampleCount() const { return m_MaxSampleCount; }

    uint32_t FindMemoryTypeIndex(uint32_t type_filter, VkMemoryPropertyFlags properties);
    VkFormatProperties GetFormatProperties(const VkFormat& format);

    VkCommandBuffer GetSingleTimeCommandBuffer();
    void SubmitSingleTimeCommandBuffer(VkCommandBuffer command_buffer);

  private:
    void CreateVulkanInstance();
    void CreateDebugMessenger();
    void CreateSurface();
    void ChoosePhysicalDevice();
    void CreateLogicalDevice();
    void CreateCommandSystem();

  private:
    VkInstance m_VulkanInstance;
#if defined(CONFIG_DEBUG) || defined(CONFIG_RELEASE)
    VkDebugUtilsMessengerEXT m_DebugMessenger;
#endif
    VkSurfaceKHR m_Surface;
    VkPhysicalDevice m_PhysicalDevice;
    VkDevice m_LogicalDevice;

    uint32_t m_GraphicsQueueFamilyIndex;
    VkQueue m_GraphicsQueue;
    uint32_t m_PresentQueueFamilyIndex;
    VkQueue m_PresentQueue;

    VkFormat m_DepthAttachmentFormat;

    VkCommandPool m_CommandPool;

    VkPhysicalDeviceMemoryProperties m_PhysicalDeviceMemoryProperties;
    VkPhysicalDeviceProperties m_PhysicalDeviceProperties;

    VkSampleCountFlagBits m_MaxSampleCount;
  };

}