#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_vulkan.cpp>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_glfw.cpp>
#include <imgui.h>

#include "Core/WindowManager.h"
#include "Rendering/DebugOverlayManager.h"
#include "Rendering/Renderer.h"

namespace yk
{

  void DebugOverlayManager::Init()
  {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(WindowManager::GetWindow(), true);

    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = Renderer::GetDevice()->GetVulkanInstance();
    initInfo.PhysicalDevice = Renderer::GetDevice()->GetPhysical();
    initInfo.Device = Renderer::GetDevice()->GetLogical();
    initInfo.RenderPass = Renderer::GetRenderPass()->Get();
    initInfo.QueueFamily = Renderer::GetDevice()->GetGraphicsQueueFamilyIndex();
    initInfo.Queue = Renderer::GetDevice()->GetGraphicsQueue();
    initInfo.PipelineCache = Renderer::GetPipelineCache()->Get();
    initInfo.DescriptorPool = Renderer::GetDescriptorPool()->Get();
    initInfo.Subpass = 0;
    initInfo.MinImageCount = Renderer::GetDevice()->GetMinSwapchainImageCount();
    initInfo.ImageCount = Renderer::GetSwapchain()->GetImageCount();
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.Allocator = VK_NULL_HANDLE;
    initInfo.CheckVkResultFn = VK_NULL_HANDLE;
    ImGui_ImplVulkan_Init(&initInfo);
  }

  void DebugOverlayManager::Shutdown()
  {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
  }

  void DebugOverlayManager::Start()
  {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
  }

  void DebugOverlayManager::Render(std::shared_ptr<CommandBuffer> buffer)
  {
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), buffer->Get());
  }

}