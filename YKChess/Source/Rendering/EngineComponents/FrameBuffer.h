#pragma once

#include <vector>
#include <optional>

#include <vulkan/vulkan.h>

#include "Rendering/EngineComponents/RenderPass.h"
#include "Rendering/Resources/DepthStencilAttachment.h"
#include "Rendering/Resources/ResolveAttachment.h"
#include "Rendering/Resources/ColorAttachment.h"
#include "Rendering/Resources/InputAttachment.h"
#include "Rendering/Resources/Attachment.h"

namespace yk
{

  struct FrameBufferData
  {
    std::vector<VkImageView> Views;
    std::optional<uint32_t> Width;
    std::optional<uint32_t> Height;
  };

  class FrameBufferStructure
  {
  public:
    FrameBufferStructure(const RenderPassData& data);

    void AddView(const std::shared_ptr<Attachment>& attachment);
    void AddView(const std::shared_ptr<ColorAttachment>& attachment);
    void AddView(const std::shared_ptr<DepthStencilAttachment>& attachment);
    void AddView(const std::shared_ptr<ResolveAttachment>& attachment);
    void AddView(const std::shared_ptr<InputAttachment>& attachment);

    const FrameBufferData& GetData() const { return m_Data; }

  private:
    bool CheckAttachmentCompatibility(const AttachmentTypeFlags& type);
    void CheckAttachmentsExtent(const VkExtent3D& extent);

  private:
    const RenderPassData& r_RenderPassData;
    FrameBufferData m_Data;
  };

  class FrameBuffer
  {
  public:
    ~FrameBuffer();

    static std::shared_ptr<FrameBuffer> Create(const std::shared_ptr<RenderPass>& rendepass, FrameBufferStructure& structure);

    const VkFramebuffer& Get() const { return m_Framebuffer; }

  private:
    std::shared_ptr<RenderPass> r_RenderPass;

    VkFramebuffer m_Framebuffer;
  };

}