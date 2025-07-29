#include <array>

#include "Core/WindowManager.h"
#include "Rendering/Swapchain.h"
#include "Rendering/Renderer.h"
#include "Rendering/Utils.h"

namespace yk
{

  Swapchain::~Swapchain()
  {
    Swapchain::Clean();
  }

  uint32_t Swapchain::AcquireNextImage(const std::shared_ptr<Semaphore>& semaphore, const std::shared_ptr<Fence>& signal_fence, uint64_t timeout)
  {
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(Renderer::GetDevice()->GetLogical(), m_Swapchain, timeout, semaphore->Get(), signal_fence ? signal_fence->Get() : VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
      Swapchain::Recreate();
      return -1;
    }
    else
      YK_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "[VULKAN] Failed to acquire the swapchain image. {}", Utils::VkResultToString(result));

    return imageIndex;
  }

  void Swapchain::Present(uint32_t image_index, const std::shared_ptr<Semaphore>& wait_semaphore)
  {
    VkSemaphore waitSemaphores[] = { wait_semaphore->Get() };
    VkSwapchainKHR swapchains[] = { m_Swapchain };

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = waitSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &image_index;
    presentInfo.pResults = VK_NULL_HANDLE;

    VkResult result = vkQueuePresentKHR(Renderer::GetDevice()->GetPresentQueue(), &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
      Swapchain::Recreate();
    else
      YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to present the swapchain image. {}", Utils::VkResultToString(result));
  }

  void Swapchain::Recreate()
  {
    glm::uvec2 framebufferSize = WindowManager::GetFramebufferSize();
    while (framebufferSize.x == 0 || framebufferSize.y == 0)
    {
      framebufferSize = WindowManager::GetFramebufferSize();
      WindowManager::WaitEvents();
    }

    Renderer::GetDevice()->WaitIdle();

    Swapchain::Clean();
    Swapchain::Create();
  }

  std::shared_ptr<Swapchain> Swapchain::Create(std::shared_ptr<RenderPass> renderpass)
  {
    std::shared_ptr<Swapchain> swapchain = std::make_shared<Swapchain>();
    swapchain->r_RenderPass = renderpass;
    swapchain->Create();
    return swapchain;
  }

  void Swapchain::Create()
  {
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Renderer::GetDevice()->GetPhysical(), Renderer::GetDevice()->GetSurface(), &surfaceCapabilities);

    // SWAPCHAIN IMAGE COUNT
    m_SwapchainImageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 && m_SwapchainImageCount > surfaceCapabilities.maxImageCount)
      m_SwapchainImageCount = surfaceCapabilities.maxImageCount;

    // SURFACE IMAGE FORMAT
    uint32_t surfaceImageFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(Renderer::GetDevice()->GetPhysical(), Renderer::GetDevice()->GetSurface(), &surfaceImageFormatCount, VK_NULL_HANDLE);
    std::vector<VkSurfaceFormatKHR> surfaceImageFormats(surfaceImageFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(Renderer::GetDevice()->GetPhysical(), Renderer::GetDevice()->GetSurface(), &surfaceImageFormatCount, surfaceImageFormats.data());
    m_SwapchainFormat = surfaceImageFormats[0];
    for (const VkSurfaceFormatKHR& surfaceFormat : surfaceImageFormats)
    {
      if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
      {
        m_SwapchainFormat = surfaceFormat;
        break;
      }
    }

    // SWAPCHAIN PRESENT MODE
    uint32_t surfacePresentModesCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(Renderer::GetDevice()->GetPhysical(), Renderer::GetDevice()->GetSurface(), &surfacePresentModesCount, VK_NULL_HANDLE);
    std::vector<VkPresentModeKHR> surfacePresentModes(surfacePresentModesCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(Renderer::GetDevice()->GetPhysical(), Renderer::GetDevice()->GetSurface(), &surfacePresentModesCount, surfacePresentModes.data());

    m_SwapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (const VkPresentModeKHR& surfacePresentMode : surfacePresentModes)
    {
      if (surfacePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
      {
        m_SwapchainPresentMode = surfacePresentMode;
        break;
      }
    }

    // SURFACE EXTENT
    VkExtent2D surfaceExtent = Renderer::GetDevice()->GetSurfaceExtent();

    // SWAPCHAIN CREATION
    VkSwapchainCreateInfoKHR swapchainCreateInfo{};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = Renderer::GetDevice()->GetSurface();
    swapchainCreateInfo.minImageCount = m_SwapchainImageCount;
    swapchainCreateInfo.imageFormat = m_SwapchainFormat.format;
    swapchainCreateInfo.imageColorSpace = m_SwapchainFormat.colorSpace;
    swapchainCreateInfo.imageExtent = surfaceExtent;
    swapchainCreateInfo.imageArrayLayers = 1; // 1 for normal rendering, more for stereoscopic/layered rendering
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = m_SwapchainPresentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if (Renderer::GetDevice()->GetGraphicsQueueFamilyIndex() != Renderer::GetDevice()->GetPresentQueueFamilyIndex())
    {
      std::array<uint32_t, 2> queueFamilyIndices = { Renderer::GetDevice()->GetGraphicsQueueFamilyIndex(), Renderer::GetDevice()->GetPresentQueueFamilyIndex() };
      swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      swapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());
      swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }
    else
    {
      swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
      swapchainCreateInfo.queueFamilyIndexCount = 0;
      swapchainCreateInfo.pQueueFamilyIndices = VK_NULL_HANDLE;
    }

    VkResult result = vkCreateSwapchainKHR(Renderer::GetDevice()->GetLogical(), &swapchainCreateInfo, VK_NULL_HANDLE, &m_Swapchain);
    YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the swapchain. Error: {}", Utils::VkResultToString(result));

    // EXTENT
    m_Extent = surfaceExtent;

    // SWAPCHAIN IMAGES AND IMAGE VIEWS
    vkGetSwapchainImagesKHR(Renderer::GetDevice()->GetLogical(), m_Swapchain, &m_SwapchainImageCount, VK_NULL_HANDLE);
    std::vector<VkImage> swapchainImages(m_SwapchainImageCount);
    vkGetSwapchainImagesKHR(Renderer::GetDevice()->GetLogical(), m_Swapchain, &m_SwapchainImageCount, swapchainImages.data());

    m_SwapchainImageViews.resize(swapchainImages.size());

    for (size_t i = 0; i < swapchainImages.size(); i++)
    {
      VkImageViewCreateInfo imageViewCreateInfo{};
      imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      imageViewCreateInfo.image = swapchainImages[i];
      imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
      imageViewCreateInfo.format = m_SwapchainFormat.format;
      imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
      imageViewCreateInfo.subresourceRange.levelCount = 1;
      imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
      imageViewCreateInfo.subresourceRange.layerCount = 1;
      imageViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };

      VkResult result = vkCreateImageView(Renderer::GetDevice()->GetLogical(), &imageViewCreateInfo, VK_NULL_HANDLE, &m_SwapchainImageViews[i]);
      YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the image view");
    }

    // DEPTH IMAGE AND IMAGE VIEW + LAYOUT TRANSITION
    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width = m_Extent.width;
    imageCreateInfo.extent.height = m_Extent.height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.format = Renderer::GetDevice()->GetDepthAttachmentFormat();
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    result = vkCreateImage(Renderer::GetDevice()->GetLogical(), &imageCreateInfo, VK_NULL_HANDLE, &m_DepthImage);
    YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the image");

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(Renderer::GetDevice()->GetLogical(), m_DepthImage, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocInfo{};
    memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocInfo.allocationSize = memoryRequirements.size;
    memoryAllocInfo.memoryTypeIndex = Renderer::GetDevice()->FindMemoryTypeIndex(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    result = vkAllocateMemory(Renderer::GetDevice()->GetLogical(), &memoryAllocInfo, VK_NULL_HANDLE, &m_DepthImageMemory);
    YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to allocate memory to the image");

    result = vkBindImageMemory(Renderer::GetDevice()->GetLogical(), m_DepthImage, m_DepthImageMemory, 0);
    YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to bind the image memory");

    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = m_DepthImage;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = Renderer::GetDevice()->GetDepthAttachmentFormat();
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;
    imageViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };

    result = vkCreateImageView(Renderer::GetDevice()->GetLogical(), &imageViewCreateInfo, VK_NULL_HANDLE, &m_DepthImageView);
    YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the image view");

    VkCommandBuffer commandBuffer = Renderer::GetDevice()->GetSingleTimeCommandBuffer();

    VkImageMemoryBarrier imageMemoryBarrier{};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.image = m_DepthImage;
    imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    imageMemoryBarrier.subresourceRange.levelCount = 1;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier.subresourceRange.layerCount = 1;
    imageMemoryBarrier.srcAccessMask = 0;
    imageMemoryBarrier.dstAccessMask = 0;

    imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    if (Renderer::GetDevice()->GetDepthAttachmentFormat() == VK_FORMAT_D32_SFLOAT_S8_UINT || Renderer::GetDevice()->GetDepthAttachmentFormat() == VK_FORMAT_D24_UNORM_S8_UINT)
      imageMemoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

    VkPipelineStageFlags pipelineSourceStage;
    VkPipelineStageFlags pipelineDestinationStage;

    imageMemoryBarrier.srcAccessMask = 0;
    imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    pipelineSourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    pipelineDestinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

    vkCmdPipelineBarrier(commandBuffer, pipelineSourceStage, pipelineDestinationStage, 0, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, 1, &imageMemoryBarrier);

    Renderer::GetDevice()->SubmitSingleTimeCommandBuffer(commandBuffer);

    // FRAMEBUFFERS
    m_Framebuffers.resize(swapchainImages.size());

    for (size_t i = 0; i < m_SwapchainImageViews.size(); i++) {
      std::array<VkImageView, 2> attachments = { m_SwapchainImageViews[i], m_DepthImageView };

      VkFramebufferCreateInfo framebufferCreateInfo{};
      framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      framebufferCreateInfo.renderPass = r_RenderPass->Get();
      framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
      framebufferCreateInfo.pAttachments = attachments.data();
      framebufferCreateInfo.width = m_Extent.width;
      framebufferCreateInfo.height = m_Extent.height;
      framebufferCreateInfo.layers = 1;

      VkResult result = vkCreateFramebuffer(Renderer::GetDevice()->GetLogical(), &framebufferCreateInfo, VK_NULL_HANDLE, &m_Framebuffers[i]);
      YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create a swapchain framebuffer. Error: {}", Utils::VkResultToString(result));
    }
  }

  void Swapchain::Clean()
  {
    vkDestroyImageView(Renderer::GetDevice()->GetLogical(), m_DepthImageView, VK_NULL_HANDLE);
    vkDestroyImage(Renderer::GetDevice()->GetLogical(), m_DepthImage, VK_NULL_HANDLE);
    vkFreeMemory(Renderer::GetDevice()->GetLogical(), m_DepthImageMemory, VK_NULL_HANDLE);

    for (VkFramebuffer framebuffer : m_Framebuffers)
      vkDestroyFramebuffer(Renderer::GetDevice()->GetLogical(), framebuffer, VK_NULL_HANDLE);

    for (VkImageView imageView : m_SwapchainImageViews)
      vkDestroyImageView(Renderer::GetDevice()->GetLogical(), imageView, VK_NULL_HANDLE);

    vkDestroySwapchainKHR(Renderer::GetDevice()->GetLogical(), m_Swapchain, VK_NULL_HANDLE);
  }
}