#pragma once

#include <string>

#include <vulkan/vulkan.h>
#include <YKLib.h>

namespace yk
{
  namespace Utils
  {
    static std::string_view VkResultToString(const VkResult& result)
    {
      switch (result)
      {
      case VK_SUCCESS:
        return "VK_SUCCESS: Command successfully completed.";
      case VK_NOT_READY:
        return "VK_NOT_READY: A fence or query has not yet completed.";
      case VK_TIMEOUT:
        return "VK_TIMEOUT: A wait operation has not completed in the specified time.";
      case VK_EVENT_SET:
        return "VK_EVENT_SET: An event is signaled.";
      case VK_EVENT_RESET:
        return "VK_EVENT_RESET: An event is unsignaled.";
      case VK_INCOMPLETE:
        return "VK_INCOMPLETE: A return array was too small for the result.";
      case VK_ERROR_OUT_OF_HOST_MEMORY:
        return "VK_ERROR_OUT_OF_HOST_MEMORY: A host memory allocation has failed.";
      case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        return "VK_ERROR_OUT_OF_DEVICE_MEMORY: A device memory allocation has failed.";
      case VK_ERROR_INITIALIZATION_FAILED:
        return "VK_ERROR_INITIALIZATION_FAILED: Initialization of an object could not be completed for implementation-specific reasons.";
      case VK_ERROR_DEVICE_LOST:
        return "VK_ERROR_DEVICE_LOST: The logical or physical device has been lost.";
      case VK_ERROR_MEMORY_MAP_FAILED:
        return "VK_ERROR_MEMORY_MAP_FAILED: Mapping of a memory object has failed.";
      case VK_ERROR_LAYER_NOT_PRESENT:
        return "VK_ERROR_LAYER_NOT_PRESENT: A requested layer is not present or could not be loaded.";
      case VK_ERROR_EXTENSION_NOT_PRESENT:
        return "VK_ERROR_EXTENSION_NOT_PRESENT: A requested extension is not supported.";
      case VK_ERROR_FEATURE_NOT_PRESENT:
        return "VK_ERROR_FEATURE_NOT_PRESENT: A requested feature is not supported.";
      case VK_ERROR_INCOMPATIBLE_DRIVER:
        return "VK_ERROR_INCOMPATIBLE_DRIVER: The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons.";
      case VK_ERROR_TOO_MANY_OBJECTS:
        return "VK_ERROR_TOO_MANY_OBJECTS: Too many objects of the type have already been created.";
      case VK_ERROR_FORMAT_NOT_SUPPORTED:
        return "VK_ERROR_FORMAT_NOT_SUPPORTED: A requested format is not supported on this device.";
      case VK_ERROR_FRAGMENTED_POOL:
        return "VK_ERROR_FRAGMENTED_POOL: A pool allocation has failed due to fragmentation of the pool's memory.";
      case VK_ERROR_OUT_OF_POOL_MEMORY:
        return "VK_ERROR_OUT_OF_POOL_MEMORY: A pool memory allocation has failed.";
      case VK_ERROR_INVALID_EXTERNAL_HANDLE:
        return "VK_ERROR_INVALID_EXTERNAL_HANDLE: An external handle is not a valid handle of the specified type.";
      case VK_ERROR_SURFACE_LOST_KHR:
        return "VK_ERROR_SURFACE_LOST_KHR: A surface is no longer available.";
      case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
        return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: The requested window is already connected to a Vulkan surface or another API.";
      case VK_SUBOPTIMAL_KHR:
        return "VK_SUBOPTIMAL_KHR: A swapchain no longer matches the surface properties exactly but can still be used.";
      case VK_ERROR_OUT_OF_DATE_KHR:
        return "VK_ERROR_OUT_OF_DATE_KHR: A surface has changed in such a way that it is no longer compatible with the swapchain.";
      case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
        return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: The display used by a swapchain does not use the same presentable image layout as the swapchain.";
      case VK_ERROR_VALIDATION_FAILED_EXT:
        return "VK_ERROR_VALIDATION_FAILED_EXT: A validation layer found an error.";
      case VK_ERROR_INVALID_SHADER_NV:
        return "VK_ERROR_INVALID_SHADER_NV: One or more shaders failed to compile or link.";
      case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
        return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: The provided DRM format modifier plane layout is invalid.";
      case VK_ERROR_FRAGMENTATION_EXT:
        return "VK_ERROR_FRAGMENTATION_EXT: A descriptor pool creation failed due to fragmentation.";
      case VK_ERROR_NOT_PERMITTED_EXT:
        return "VK_ERROR_NOT_PERMITTED_EXT: The operation is not permitted.";
      case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:
        return "VK_ERROR_INVALID_DEVICE_ADDRESS_EXT: The provided device address is not a valid buffer address.";
      case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
        return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT: The swapchain has lost full-screen exclusivity.";
      case VK_THREAD_IDLE_KHR:
        return "VK_THREAD_IDLE_KHR: A thread is waiting for work.";
      case VK_THREAD_DONE_KHR:
        return "VK_THREAD_DONE_KHR: A thread is done executing.";
      case VK_OPERATION_DEFERRED_KHR:
        return "VK_OPERATION_DEFERRED_KHR: The requested operation was deferred.";
      case VK_OPERATION_NOT_DEFERRED_KHR:
        return "VK_OPERATION_NOT_DEFERRED_KHR: The requested operation was not deferred.";
      case VK_PIPELINE_COMPILE_REQUIRED_EXT:
        return "VK_PIPELINE_COMPILE_REQUIRED_EXT: A pipeline needs to be recompiled.";
      case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:
        return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR: The requested image usage is not supported.";
      default:
        return "VK_UNKNOWN_ERROR: An unknown Vulkan error has occurred.";
      }
    }

    static uint32_t FormatToByteSize(const VkFormat& format)
    {
      switch (format)
      {
      case VK_FORMAT_R4G4_UNORM_PACK8: return 1;

      case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
      case VK_FORMAT_B4G4R4A4_UNORM_PACK16: return 2;

      case VK_FORMAT_R5G6B5_UNORM_PACK16:
      case VK_FORMAT_B5G6R5_UNORM_PACK16: return 2;

      case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
      case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
      case VK_FORMAT_A1R5G5B5_UNORM_PACK16: return 2;

      case VK_FORMAT_R8_UNORM:
      case VK_FORMAT_R8_SNORM:
      case VK_FORMAT_R8_USCALED:
      case VK_FORMAT_R8_SSCALED:
      case VK_FORMAT_R8_UINT:
      case VK_FORMAT_R8_SINT:
      case VK_FORMAT_R8_SRGB: return 1;

      case VK_FORMAT_R8G8_UNORM:
      case VK_FORMAT_R8G8_SNORM:
      case VK_FORMAT_R8G8_USCALED:
      case VK_FORMAT_R8G8_SSCALED:
      case VK_FORMAT_R8G8_UINT:
      case VK_FORMAT_R8G8_SINT:
      case VK_FORMAT_R8G8_SRGB: return 2;

      case VK_FORMAT_R8G8B8_UNORM:
      case VK_FORMAT_R8G8B8_SNORM:
      case VK_FORMAT_R8G8B8_USCALED:
      case VK_FORMAT_R8G8B8_SSCALED:
      case VK_FORMAT_R8G8B8_UINT:
      case VK_FORMAT_R8G8B8_SINT:
      case VK_FORMAT_R8G8B8_SRGB:
      case VK_FORMAT_B8G8R8_UNORM:
      case VK_FORMAT_B8G8R8_SNORM:
      case VK_FORMAT_B8G8R8_USCALED:
      case VK_FORMAT_B8G8R8_SSCALED:
      case VK_FORMAT_B8G8R8_UINT:
      case VK_FORMAT_B8G8R8_SINT:
      case VK_FORMAT_B8G8R8_SRGB: return 3;

      case VK_FORMAT_R8G8B8A8_UNORM:
      case VK_FORMAT_R8G8B8A8_SNORM:
      case VK_FORMAT_R8G8B8A8_USCALED:
      case VK_FORMAT_R8G8B8A8_SSCALED:
      case VK_FORMAT_R8G8B8A8_UINT:
      case VK_FORMAT_R8G8B8A8_SINT:
      case VK_FORMAT_R8G8B8A8_SRGB:
      case VK_FORMAT_B8G8R8A8_UNORM:
      case VK_FORMAT_B8G8R8A8_SNORM:
      case VK_FORMAT_B8G8R8A8_USCALED:
      case VK_FORMAT_B8G8R8A8_SSCALED:
      case VK_FORMAT_B8G8R8A8_UINT:
      case VK_FORMAT_B8G8R8A8_SINT:
      case VK_FORMAT_B8G8R8A8_SRGB:
      case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
      case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
      case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
      case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
      case VK_FORMAT_A8B8G8R8_UINT_PACK32:
      case VK_FORMAT_A8B8G8R8_SINT_PACK32:
      case VK_FORMAT_A8B8G8R8_SRGB_PACK32: return 4;

      case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
      case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
      case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
      case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
      case VK_FORMAT_A2R10G10B10_UINT_PACK32:
      case VK_FORMAT_A2R10G10B10_SINT_PACK32:
      case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
      case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
      case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
      case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
      case VK_FORMAT_A2B10G10R10_UINT_PACK32:
      case VK_FORMAT_A2B10G10R10_SINT_PACK32: return 4;

      case VK_FORMAT_R16_UNORM:
      case VK_FORMAT_R16_SNORM:
      case VK_FORMAT_R16_USCALED:
      case VK_FORMAT_R16_SSCALED:
      case VK_FORMAT_R16_UINT:
      case VK_FORMAT_R16_SINT:
      case VK_FORMAT_R16_SFLOAT: return 2;

      case VK_FORMAT_R16G16_UNORM:
      case VK_FORMAT_R16G16_SNORM:
      case VK_FORMAT_R16G16_USCALED:
      case VK_FORMAT_R16G16_SSCALED:
      case VK_FORMAT_R16G16_UINT:
      case VK_FORMAT_R16G16_SINT:
      case VK_FORMAT_R16G16_SFLOAT: return 4;

      case VK_FORMAT_R16G16B16_UNORM:
      case VK_FORMAT_R16G16B16_SNORM:
      case VK_FORMAT_R16G16B16_USCALED:
      case VK_FORMAT_R16G16B16_SSCALED:
      case VK_FORMAT_R16G16B16_UINT:
      case VK_FORMAT_R16G16B16_SINT:
      case VK_FORMAT_R16G16B16_SFLOAT: return 6;

      case VK_FORMAT_R16G16B16A16_UNORM:
      case VK_FORMAT_R16G16B16A16_SNORM:
      case VK_FORMAT_R16G16B16A16_USCALED:
      case VK_FORMAT_R16G16B16A16_SSCALED:
      case VK_FORMAT_R16G16B16A16_UINT:
      case VK_FORMAT_R16G16B16A16_SINT:
      case VK_FORMAT_R16G16B16A16_SFLOAT: return 8;

      case VK_FORMAT_R32_UINT:
      case VK_FORMAT_R32_SINT:
      case VK_FORMAT_R32_SFLOAT: return 4;

      case VK_FORMAT_R32G32_UINT:
      case VK_FORMAT_R32G32_SINT:
      case VK_FORMAT_R32G32_SFLOAT: return 8;

      case VK_FORMAT_R32G32B32_UINT:
      case VK_FORMAT_R32G32B32_SINT:
      case VK_FORMAT_R32G32B32_SFLOAT: return 12;

      case VK_FORMAT_R32G32B32A32_UINT:
      case VK_FORMAT_R32G32B32A32_SINT:
      case VK_FORMAT_R32G32B32A32_SFLOAT: return 16;

      case VK_FORMAT_R64_UINT:
      case VK_FORMAT_R64_SINT:
      case VK_FORMAT_R64_SFLOAT: return 8;

      case VK_FORMAT_R64G64_UINT:
      case VK_FORMAT_R64G64_SINT:
      case VK_FORMAT_R64G64_SFLOAT: return 16;

      case VK_FORMAT_R64G64B64_UINT:
      case VK_FORMAT_R64G64B64_SINT:
      case VK_FORMAT_R64G64B64_SFLOAT: return 24;

      case VK_FORMAT_R64G64B64A64_UINT:
      case VK_FORMAT_R64G64B64A64_SINT:
      case VK_FORMAT_R64G64B64A64_SFLOAT: return 32;

        // Depth formats
      case VK_FORMAT_D16_UNORM: return 2;
      case VK_FORMAT_D32_SFLOAT: return 4;

        // Stencil formats
      case VK_FORMAT_S8_UINT: return 1;

        // Depth/stencil formats
      case VK_FORMAT_D16_UNORM_S8_UINT: return 3; // 2 bytes for depth, 1 byte for stencil
      case VK_FORMAT_D24_UNORM_S8_UINT: return 4; // 3 bytes for depth, 1 byte for stencil
      case VK_FORMAT_D32_SFLOAT_S8_UINT: return 5; // 4 bytes for depth, 1 byte for stencil

      default: YK_ASSERT(false, "System:  Invalid or not-supported VkFormat specified");
      }
    }
  }
}
