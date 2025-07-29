#pragma once

#include <filesystem>
#include <memory>

#include <vulkan/vulkan.h>

namespace yk
{

  class Shader
  {
  public:
    enum class Type
    {
      Vertex,
      TessellationControl,
      TessellationEvaluation,
      Geometry,
      Fragment,
      Compute,
      RayGeneration,
      AnyHit,
      ClosestHit,
      Miss,
      Intersection,
      Callable,
      Task,
      Mesh
    };

  public:
    ~Shader();

    const VkShaderModule& GetModule() const { return m_Module; }
    const Shader::Type& GetType() const { return m_Type; }

    static std::shared_ptr<Shader> Create(const std::filesystem::path& filepath);

  private:
    Shader::Type ShaderTypeFromFileExtension(const std::filesystem::path& filepath);

  private:
    VkShaderModule m_Module = VK_NULL_HANDLE;
    Shader::Type m_Type;
    std::filesystem::path m_Filepath;
  };

}