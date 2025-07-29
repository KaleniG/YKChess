#pragma once

#include <filesystem>
#include <vector>

#include <vulkan/vulkan.h>

namespace yk
{

  class PipelineCache
  {
  public:
    ~PipelineCache();

    const VkPipelineCache& Get() const { return m_Cache; }

    static std::shared_ptr<PipelineCache> Create(const std::filesystem::path& filepath);

  private:
    VkPipelineCache m_Cache = VK_NULL_HANDLE;
    std::vector<char> m_CacheData;
    std::filesystem::path m_Filepath;
  };

}