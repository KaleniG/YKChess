#include <set>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan.h>

#include <YKLib.h>

#include "Core/WindowManager.h"
#include "Rendering/Device.h"
#include "Rendering/Utils.h"

namespace yk
{
  VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data)
  {
    switch (message_severity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    {
      YK_INFO("Vulkan {}", callback_data->pMessage);
      break;
    }
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
    {
      YK_INFO("Vulkan {}", callback_data->pMessage);
      break;
    }
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
    {
      YK_WARN("Vulkan {}", callback_data->pMessage);
      break;
    }
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    {
      YK_ERROR("Vulkan {}", callback_data->pMessage);
      break;
    }
    }

    return VK_FALSE;
  }

  VkResult CreateDebugUtilsMessenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* create_info, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debug_messenger)
  {
    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

    if (func != nullptr)
    {
      func(instance, create_info, allocator, debug_messenger);
      return VK_SUCCESS;
    }
    else
    {
      YK_ERROR("Vulkan: 'vkCreateDebugUtilsMessengerEXT' isn't present");
      return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
  }

  void DestroyDebugUtilsMessenger(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* allocator)
  {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

    if (func != nullptr)
      func(instance, debug_messenger, allocator);
  }

  struct DeviceReqs
  {
    std::vector<const char*> Layers =
    {
      #if defined(CONFIG_DEBUG) || defined(CONFIG_RELEASE)
      "VK_LAYER_KHRONOS_validation"
      #endif
    };

    std::vector<const char*> InstanceExtensions =
    {
      "VK_KHR_surface",
      "VK_KHR_win32_surface",
      #if defined(CONFIG_DEBUG) || defined(CONFIG_RELEASE)
      "VK_EXT_debug_utils"
      #endif
    };

    std::vector<const char*> DeviceExtensions =
    {
      "VK_KHR_swapchain"
    };

  };

  static DeviceReqs* s_DeviceReqs = new DeviceReqs;

  Device::Device()
  {
    Device::CreateVulkanInstance();
    Device::CreateDebugMessenger();
    Device::CreateSurface();
    Device::ChoosePhysicalDevice();
    Device::CreateLogicalDevice();
    Device::CreateCommandSystem();
  }

  Device::~Device()
  {
    vkDestroyCommandPool(m_LogicalDevice, m_CommandPool, VK_NULL_HANDLE);
    vkDestroyDevice(m_LogicalDevice, VK_NULL_HANDLE);
    vkDestroySurfaceKHR(m_VulkanInstance, m_Surface, VK_NULL_HANDLE);
#if defined(CONFIG_DEBUG) || defined(CONFIG_RELEASE)
    DestroyDebugUtilsMessenger(m_VulkanInstance, m_DebugMessenger, VK_NULL_HANDLE);
#endif
    vkDestroyInstance(m_VulkanInstance, VK_NULL_HANDLE);
  }

  void Device::WaitIdle()
  {
    vkDeviceWaitIdle(m_LogicalDevice);
  }

  uint32_t Device::GetMinSwapchainImageCount()
  {
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &surfaceCapabilities);
    return surfaceCapabilities.minImageCount;
  }

  const VkExtent2D& Device::GetSurfaceExtent() const
  {
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &surfaceCapabilities);

    if (surfaceCapabilities.currentExtent.width != static_cast<uint32_t>(std::numeric_limits<uint32_t>::max()))
      return surfaceCapabilities.currentExtent;
    else
    {
      glm::uvec2 framebufferExtent = WindowManager::GetFramebufferSize();

      VkExtent2D actualExtent;
      actualExtent.width = static_cast<uint32_t>(framebufferExtent.x);
      actualExtent.height = static_cast<uint32_t>(framebufferExtent.y);

      actualExtent.width = std::clamp(actualExtent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
      actualExtent.height = std::clamp(actualExtent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);

      return actualExtent;
    }
  }

  void Device::CreateVulkanInstance()
  {
    // LAYERS SUPPORT
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, VK_NULL_HANDLE);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    bool layersSupport = true;
    for (const char* requiredLayer : s_DeviceReqs->Layers)
    {
      bool layerSupported = false;
      for (const VkLayerProperties& availableLayer : availableLayers)
      {
        if (std::strcmp(requiredLayer, availableLayer.layerName) == 0)
        {
          layerSupported = true;
          break;
        }
      }

      layersSupport = layersSupport && layerSupported;

      if (layerSupported)
        continue;

      YK_ERROR("Vulkan: '{}' layer is not supported or not-existent", requiredLayer);
    }
    YK_ASSERT(layersSupport, "Vulkan: Invalid Vulkan layers support");

    // INSTANCE EXTENSION SUPPORT
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &extensionCount, VK_NULL_HANDLE);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &extensionCount, availableExtensions.data());

    bool extensionsSupport = true;
    for (const char* requiredExtension : s_DeviceReqs->InstanceExtensions)
    {
      bool extensionSupported = false;
      for (const VkExtensionProperties& availableExtension : availableExtensions)
      {
        if (std::strcmp(requiredExtension, availableExtension.extensionName) == 0)
        {
          extensionSupported = true;
          break;
        }
      }
      extensionsSupport = extensionsSupport && extensionSupported;

      if (extensionSupported)
        continue;

      YK_ERROR("Vulkan: '{}' extension is not supported or not-existent", requiredExtension);
    }
    YK_ASSERT(extensionsSupport, "Vulkan: Invalid Vulkan extension support");

    // EVERYTHING SUPPORTED
    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = "None";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    applicationInfo.pEngineName = "None";
    applicationInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(s_DeviceReqs->InstanceExtensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = s_DeviceReqs->InstanceExtensions.data();
    instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(s_DeviceReqs->Layers.size());
    instanceCreateInfo.ppEnabledLayerNames = s_DeviceReqs->Layers.data();

    #if defined(CONFIG_DEBUG) || defined(CONFIG_RELEASE)
    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo{};
    debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugUtilsMessengerCreateInfo.pfnUserCallback = debug_utils_messenger_callback;
    debugUtilsMessengerCreateInfo.pUserData = VK_NULL_HANDLE;
    instanceCreateInfo.pNext = &debugUtilsMessengerCreateInfo;
    #else
    instanceCreateInfo.pNext = VK_NULL_HANDLE;
    #endif

    VkResult result = vkCreateInstance(&instanceCreateInfo, VK_NULL_HANDLE, &m_VulkanInstance);
    YK_ASSERT(result == VK_SUCCESS, "Vulkan: Failed to create the Vulkan instance. {}", Utils::VkResultToString(result));
  }

  void Device::CreateDebugMessenger()
  {
    #if defined(CONFIG_DEBUG) || defined(CONFIG_RELEASE)
    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo{};
    debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugUtilsMessengerCreateInfo.pfnUserCallback = debug_utils_messenger_callback;
    debugUtilsMessengerCreateInfo.pUserData = VK_NULL_HANDLE;

    VkResult result = CreateDebugUtilsMessenger(m_VulkanInstance, &debugUtilsMessengerCreateInfo, VK_NULL_HANDLE, &m_DebugMessenger);
    YK_ASSERT(result == VK_SUCCESS, "Vulkan: Failed to create the debug messenger. {}", Utils::VkResultToString(result));
    #endif
  }

  void Device::CreateSurface()
  {
    VkResult result = glfwCreateWindowSurface(m_VulkanInstance, WindowManager::GetWindow(), VK_NULL_HANDLE, &m_Surface);
    YK_ASSERT(result == VK_SUCCESS, "GLFW/Vulkan: failed to create a surface. {}", Utils::VkResultToString(result));
  }

  void Device::ChoosePhysicalDevice()
  {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_VulkanInstance, &deviceCount, VK_NULL_HANDLE);
    YK_ASSERT(deviceCount != 0, "Vulkan: failed to find a physical device");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_VulkanInstance, &deviceCount, devices.data());

    #if defined(CONFIG_DEBUG) || defined(CONFIG_RELEASE)
    std::string physicalDevicesStr;
    for (uint32_t i = 0; i < deviceCount; i++)
    {
      VkPhysicalDeviceProperties deviceProperties;
      vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
      physicalDevicesStr.append(" (");
      physicalDevicesStr.append(deviceProperties.deviceName);
      physicalDevicesStr.append(")");
    }
    YK_INFO("Vulkan: found {} physical devices: {}", deviceCount, physicalDevicesStr);
    #endif

    for (const VkPhysicalDevice& device : devices)
    {
      // QUEUE SUPPORT
      uint32_t queueFamilyCount = 0;
      vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, VK_NULL_HANDLE);
      std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
      vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

      std::optional<uint32_t> graphicsQueueIndex;
      std::optional<uint32_t> presentQueueIndex;

      int i = 0;
      for (const VkQueueFamilyProperties& queueFamily : queueFamilies)
      {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
          graphicsQueueIndex = i;

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);

        if (presentSupport)
          presentQueueIndex = i;

        if (graphicsQueueIndex.has_value() && presentQueueIndex.has_value())
          break;

        i++;
      }

      bool queueSupport = graphicsQueueIndex.has_value() && presentQueueIndex.has_value();
      if (!queueSupport)
        continue;

      // FEATURE SUPPORT
      VkPhysicalDeviceFeatures supportedFeatures;
      vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
      bool featureSupport = supportedFeatures.samplerAnisotropy;
      if (!featureSupport)
        continue;

      // SWAPCHAIN SUPPORT
      std::vector<VkSurfaceFormatKHR> surfaceFormats;
      uint32_t formatCount;
      vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, VK_NULL_HANDLE);
      surfaceFormats.resize(formatCount);
      vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, surfaceFormats.data());

      std::vector<VkPresentModeKHR> presentModes;
      uint32_t presentModeCount;
      vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, VK_NULL_HANDLE);
      presentModes.resize(presentModeCount);
      vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, presentModes.data());

      bool swapchainSupport = !surfaceFormats.empty() && !presentModes.empty();
      if (!swapchainSupport)
        continue;

      // DEVICE EXTENSION SUPPORT
      uint32_t extensionCount;
      vkEnumerateDeviceExtensionProperties(device, VK_NULL_HANDLE, &extensionCount, VK_NULL_HANDLE);
      std::vector<VkExtensionProperties> availableExtensions(extensionCount);
      vkEnumerateDeviceExtensionProperties(device, VK_NULL_HANDLE, &extensionCount, availableExtensions.data());

      bool extensionSupport = true;
      for (const char* requiredExtension : s_DeviceReqs->DeviceExtensions)
      {
        bool extensionSupported = false;
        for (const VkExtensionProperties& availableExtension : availableExtensions)
        {
          if (std::strcmp(requiredExtension, availableExtension.extensionName) == 0)
          {
            extensionSupported = true;
            break;
          }
        }
        extensionSupport = extensionSupport && extensionSupported;

        if (extensionSupported)
          continue;

        YK_ERROR("Vulkan: '{}' device extension is not supported or not-existent", requiredExtension);
      }

      if (extensionSupport)
      {
        m_PhysicalDevice = device;
        m_GraphicsQueueFamilyIndex = graphicsQueueIndex.value();
        m_PresentQueueFamilyIndex = presentQueueIndex.value();
        break;
      }
    }

    YK_ASSERT(m_PhysicalDevice != VK_NULL_HANDLE, "Vulkan: failed to find a suitable physical device");
  }

  void Device::CreateLogicalDevice()
  {
    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_PhysicalDeviceProperties);
    vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &m_PhysicalDeviceMemoryProperties);

    // MAX SAMPLE COUNT
    VkSampleCountFlags counts = m_PhysicalDeviceProperties.limits.framebufferColorSampleCounts & m_PhysicalDeviceProperties.limits.framebufferDepthSampleCounts;
    m_MaxSampleCount = VK_SAMPLE_COUNT_1_BIT;
    if (counts & VK_SAMPLE_COUNT_64_BIT) { m_MaxSampleCount = VK_SAMPLE_COUNT_64_BIT; }
    else if (counts & VK_SAMPLE_COUNT_32_BIT) { m_MaxSampleCount = VK_SAMPLE_COUNT_32_BIT; }
    else if (counts & VK_SAMPLE_COUNT_16_BIT) { m_MaxSampleCount = VK_SAMPLE_COUNT_16_BIT; }
    else if (counts & VK_SAMPLE_COUNT_8_BIT) { m_MaxSampleCount = VK_SAMPLE_COUNT_8_BIT; }
    else if (counts & VK_SAMPLE_COUNT_4_BIT) { m_MaxSampleCount = VK_SAMPLE_COUNT_4_BIT; }
    else if (counts & VK_SAMPLE_COUNT_2_BIT) { m_MaxSampleCount = VK_SAMPLE_COUNT_2_BIT; }

    std::set<uint32_t> uniqueQueueFamilies = { m_GraphicsQueueFamilyIndex, m_PresentQueueFamilyIndex };
    std::vector<VkDeviceQueueCreateInfo> logicalDeviceQueueCreateInfos;

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
      VkDeviceQueueCreateInfo logicalDeviceQueueCreateInfo{};
      logicalDeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      logicalDeviceQueueCreateInfo.queueFamilyIndex = queueFamily;
      logicalDeviceQueueCreateInfo.queueCount = 1;
      logicalDeviceQueueCreateInfo.pQueuePriorities = &queuePriority;
      logicalDeviceQueueCreateInfos.push_back(logicalDeviceQueueCreateInfo);
    }

    VkPhysicalDeviceFeatures physicalDeviceFeatures = {};
    physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
    physicalDeviceFeatures.fillModeNonSolid = VK_TRUE; // HERE ONLY BECAUSE OF THE PIPELINE THE Renderer3D

    VkDeviceCreateInfo logicalDeviceCreateInfo{};
    logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    logicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(logicalDeviceQueueCreateInfos.size());
    logicalDeviceCreateInfo.pQueueCreateInfos = logicalDeviceQueueCreateInfos.data();
    logicalDeviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
    logicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(s_DeviceReqs->DeviceExtensions.size());
    logicalDeviceCreateInfo.ppEnabledExtensionNames = s_DeviceReqs->DeviceExtensions.data();

    VkResult result = vkCreateDevice(m_PhysicalDevice, &logicalDeviceCreateInfo, VK_NULL_HANDLE, &m_LogicalDevice);
    YK_ASSERT(result == VK_SUCCESS, "Vulkan: failed to create the logical device. {}", Utils::VkResultToString(result));

    vkGetDeviceQueue(m_LogicalDevice, m_GraphicsQueueFamilyIndex, 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_LogicalDevice, m_PresentQueueFamilyIndex, 0, &m_PresentQueue);

    delete s_DeviceReqs;
    s_DeviceReqs = nullptr;

    /////////////////////////////////////////// TO REDO

    std::array<VkFormat, 3> depthFormats =
    {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    bool foundDepthFormat = false;
    for (VkFormat format : depthFormats)
    {
      VkFormatProperties formatProperties;
      vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &formatProperties);

      if ((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
      {
        m_DepthAttachmentFormat = format;
        foundDepthFormat = true;
        break;
      }
    }
    YK_ASSERT(foundDepthFormat, "Vulkan: failed to find a supported depth attachment format");
  }

  void Device::CreateCommandSystem()
  {
    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = m_GraphicsQueueFamilyIndex;

    VkResult result = vkCreateCommandPool(m_LogicalDevice, &commandPoolCreateInfo, VK_NULL_HANDLE, &m_CommandPool);
    YK_ASSERT(result == VK_SUCCESS, "Vulkan: failed to create the command pool. {}", Utils::VkResultToString(result));
  }

  uint32_t Device::FindMemoryTypeIndex(uint32_t type_filter, VkMemoryPropertyFlags properties)
  {
    for (int i = 0; i < m_PhysicalDeviceMemoryProperties.memoryTypeCount; i++)
      if (type_filter & (1 << i) && (m_PhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
        return i;

    YK_ASSERT(false, "Vulkan: failed to find a required memory type");
  }

  VkFormatProperties Device::GetFormatProperties(const VkFormat& format)
  {
    VkFormatProperties properties;
    vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &properties);
    return properties;
  }

  VkCommandBuffer Device::GetSingleTimeCommandBuffer()
  {
    VkCommandBufferAllocateInfo commandBufferAllocInfo{};
    commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocInfo.commandPool = m_CommandPool;
    commandBufferAllocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    VkResult result = vkAllocateCommandBuffers(m_LogicalDevice, &commandBufferAllocInfo, &commandBuffer);
    YK_ASSERT(result == VK_SUCCESS, "Vulkan: failed to allocate command buffer. Error: {}", Utils::VkResultToString(result));

    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    result = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
    YK_ASSERT(result == VK_SUCCESS, "Vulkan: failed to begin command buffer. Error: {}", Utils::VkResultToString(result));

    return commandBuffer;
  }

  void Device::SubmitSingleTimeCommandBuffer(VkCommandBuffer commandBuffer)
  {
    VkResult result = vkEndCommandBuffer(commandBuffer);
    YK_ASSERT(result == VK_SUCCESS, "Vulkan: failed to end command buffer. Error: {}", Utils::VkResultToString(result));

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    result = vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    YK_ASSERT(result == VK_SUCCESS, "Vulkan: failed to submit command buffer. Error: {}", Utils::VkResultToString(result));

    result = vkQueueWaitIdle(m_GraphicsQueue);
    YK_ASSERT(result == VK_SUCCESS, "Vulkan: failed to wait for queue idle. Error: {}", Utils::VkResultToString(result));

    vkFreeCommandBuffers(m_LogicalDevice, m_CommandPool, 1, &commandBuffer);
  }
}