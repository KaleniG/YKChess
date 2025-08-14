#pragma once

#include <vector>
#include <memory>

#include <vulkan/vulkan.h>

#include "Rendering/EngineComponents/RenderPass.h"
#include "Rendering/EngineComponents/Semaphore.h"
#include "Rendering/EngineComponents/Fence.h"
#include "Rendering/Device.h"

namespace yk
{
  class Swapchain
  {
  public:
    ~Swapchain();

    const VkSwapchainKHR& Get() const { return m_Swapchain; }
    const std::vector<VkFramebuffer>& GetFramebuffers() const { return m_Framebuffers; }
    const VkExtent2D& GetExtent() const { return m_Extent; }
    const uint32_t& GetImageCount() const { return m_SwapchainImageCount; }

    uint32_t AcquireNextImage(const std::shared_ptr<Semaphore>& semaphore, const std::shared_ptr<Fence>& signal_fence = nullptr, uint64_t timeout = UINT64_MAX); // Not sure about signal_fence
    void Present(uint32_t image_index, const std::shared_ptr<Semaphore>& wait_semaphore);

    void Recreate();

    static std::shared_ptr<Swapchain> Create(std::shared_ptr<RenderPass> renderpass);

  private:
    void Create();
    void Clean();

  private:
    std::shared_ptr<RenderPass> r_RenderPass;

    VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;

    // MSAA state
    VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;

    // Multisampled COLOR attachment (offscreen)
    VkImage        m_ColorMsaaImage = VK_NULL_HANDLE;
    VkDeviceMemory m_ColorMsaaMemory = VK_NULL_HANDLE;
    VkImageView    m_ColorMsaaView = VK_NULL_HANDLE;

    // Depth attachment (matches m_MsaaSamples)
    VkImage        m_DepthImage = VK_NULL_HANDLE;
    VkDeviceMemory m_DepthImageMemory = VK_NULL_HANDLE;
    VkImageView    m_DepthImageView = VK_NULL_HANDLE;

    std::vector<VkImageView> m_SwapchainImageViews;
    std::vector<VkFramebuffer> m_Framebuffers;

    uint32_t m_SwapchainImageCount = 0;
    VkSurfaceFormatKHR m_SwapchainFormat{};
    VkPresentModeKHR m_SwapchainPresentMode{};
    VkExtent2D m_Extent{};
  };

}