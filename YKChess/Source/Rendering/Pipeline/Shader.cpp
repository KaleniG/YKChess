#include "Rendering/Pipeline/Shader.h"
#include "Rendering/Renderer.h"
#include "Rendering/Utils.h"

namespace yk
{

  Shader::~Shader()
  {
    vkDestroyShaderModule(Renderer::GetDevice()->GetLogical(), m_Module, VK_NULL_HANDLE);
  }

  std::shared_ptr<Shader> Shader::Create(const std::filesystem::path& filepath)
  {
    std::shared_ptr<Shader> shader = std::make_shared<Shader>();

    shader->m_Filepath = filepath;
    shader->m_Type = shader->ShaderTypeFromFileExtension(filepath);

    std::ifstream shaderFile(filepath.string(), std::ios::ate | std::ios::binary);
    YK_ASSERT(shaderFile.is_open(), "[STD] Couldn't open the shader file: {}", filepath.string());

    size_t fileSize = static_cast<size_t>(shaderFile.tellg());

    std::vector<char> shaderCode(fileSize);

    shaderFile.seekg(0);
    shaderFile.read(shaderCode.data(), fileSize);
    shaderFile.close();

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shaderCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

    VkResult result = vkCreateShaderModule(Renderer::GetDevice()->GetLogical(), &createInfo, VK_NULL_HANDLE, &shader->m_Module);
    YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the vertex shader module. Error: {}", Utils::VkResultToString(result));

    return shader;
  }

  Shader::Type Shader::ShaderTypeFromFileExtension(const std::filesystem::path& filepath)
  {
    std::string extension;
    {
      std::string filepathStr = filepath.string();
      extension = filepathStr.substr(filepathStr.find_first_of("."));
      extension = extension.substr(0, extension.find_last_of("."));
    }

    if (extension == ".vert")
      return Shader::Type::Vertex;
    if (extension == ".tesc")
      return Shader::Type::TessellationControl;
    if (extension == ".tese")
      return Shader::Type::TessellationEvaluation;
    if (extension == ".geom")
      return Shader::Type::Geometry;
    if (extension == ".frag")
      return Shader::Type::Fragment;
    if (extension == ".comp")
      return Shader::Type::Compute;
    if (extension == ".rgen")
      return Shader::Type::RayGeneration;
    if (extension == ".rahit")
      return Shader::Type::AnyHit;
    if (extension == ".rchit")
      return Shader::Type::ClosestHit;
    if (extension == ".rmiss")
      return Shader::Type::Miss;
    if (extension == ".rint")
      return Shader::Type::Intersection;
    if (extension == ".rcall")
      return Shader::Type::Callable;
    if (extension == ".task")
      return Shader::Type::Task;
    if (extension == ".mesh")
      return Shader::Type::Mesh;

    return Shader::Type::Vertex;
  }

}