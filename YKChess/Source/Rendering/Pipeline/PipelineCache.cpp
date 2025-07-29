#include "Rendering/Pipeline/PipelineCache.h"
#include "Rendering/Renderer.h"
#include "Rendering/Utils.h"

namespace yk
{

  PipelineCache::~PipelineCache()
  {
    size_t cacheDataSize;
    VkResult result = vkGetPipelineCacheData(Renderer::GetDevice()->GetLogical(), m_Cache, &cacheDataSize, VK_NULL_HANDLE);
    YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to get the pipeline chache data. Error: {}", Utils::VkResultToString(result));

    if (cacheDataSize > 0)
    {
      std::vector<char> cacheData(cacheDataSize);
      result = vkGetPipelineCacheData(Renderer::GetDevice()->GetLogical(), m_Cache, &cacheDataSize, cacheData.data());
      YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to get the pipeline chache data. Error: {}", Utils::VkResultToString(result));

      std::ofstream cacheFile(m_Filepath, std::ios::out | std::ios::binary);
      YK_ASSERT(cacheFile.is_open(), "[STD] Couldn't open the cache file: {}", m_Filepath.string());

      cacheFile.write(cacheData.data(), cacheData.size()); 
      cacheFile.close();
    }
    
    vkDestroyPipelineCache(Renderer::GetDevice()->GetLogical(), m_Cache, VK_NULL_HANDLE);
  }

  std::shared_ptr<PipelineCache> PipelineCache::Create(const std::filesystem::path& filepath)
  {
    std::shared_ptr<PipelineCache> cache = std::make_shared<PipelineCache>();

    cache->m_Filepath = filepath;

    if (!std::filesystem::exists(filepath))
    {
      std::ofstream createFile(filepath, std::ios::binary);
      //YK_ASSERT(createFile.is_open(), "[STD] Couldn't create the cache file: {}", filepath.c_str());
      createFile.close();
    }

    std::ifstream cacheFile(filepath, std::ios::ate | std::ios::binary);
    //YK_ASSERT(cacheFile.is_open(), "[STD] Couldn't open the cache file: {}", filepath);

    size_t fileSize = static_cast<size_t>(cacheFile.tellg());
    cache->m_CacheData.resize(fileSize);

    cacheFile.seekg(0);
    cacheFile.read(cache->m_CacheData.data(), fileSize);
    cacheFile.close();

    VkPipelineCacheCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    createInfo.initialDataSize = static_cast<uint32_t>(cache->m_CacheData.size());
    createInfo.pInitialData = cache->m_CacheData.data();

    VkResult result = vkCreatePipelineCache(Renderer::GetDevice()->GetLogical(), &createInfo, VK_NULL_HANDLE, &cache->m_Cache);
    YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the pipeline chache. Error: {}", Utils::VkResultToString(result));

    return cache;
  }

}