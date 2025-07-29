#include <YKLib.h>

namespace yk
{
  template<typename T>
  void PipelineShaders::AddSpecializationConstantBlock(const T& data)
  {
    YK_ASSERT(!m_ShaderStages.empty(), "[SYSTEM] Cannot add specialization constants, need to add a shader first");

    uint32_t id = 0;
    uint32_t offset = 0;
    std::vector<VkSpecializationMapEntry> mapEntries(boost::pfr::tuple_size_v<T>);

    boost::pfr::for_each_field(data, [&](const auto& field)
      {
        mapEntries[id] = { id, offset, sizeof(field) };
        offset += sizeof(field);
        id++;
      });

    m_SpecializationInfos.push_back({});
    VkSpecializationInfo& specInfo = m_SpecializationInfos.back(); 
    specInfo.pData = new T(data);
    specInfo.dataSize = sizeof(T);
    specInfo.mapEntryCount = static_cast<uint32_t>(mapEntries.size());
    specInfo.pMapEntries = mapEntries.data();

    m_MapEntries.push_back(std::move(mapEntries));
    m_ShaderStages.back().pSpecializationInfo = &m_SpecializationInfos.back();
  }
}