#include "Rendering/EngineComponents/FrameBuffer.h"
#include "Rendering/Renderer.h"
#include "Rendering/Utils.h"

namespace yk
{

  FrameBufferStructure::FrameBufferStructure(const RenderPassData& data)
    : r_RenderPassData(data) {}

  void FrameBufferStructure::AddView(const std::shared_ptr<Attachment>& attachment)
  {
    YK_ASSERT(FrameBufferStructure::CheckAttachmentCompatibility(attachment->GetAttachmentType()), "[SYSTEM/VULKAN] Invalid attachment in-place");
    YK_ASSERT(r_RenderPassData.Attachments[m_Data.Views.size()].format == attachment->GetFormat(), "[SYSTEM/VULKAN] Invalid attachment format");
    FrameBufferStructure::CheckAttachmentsExtent(attachment->GetExtent());

    m_Data.Views.push_back(attachment->GetView());
  }

  void FrameBufferStructure::AddView(const std::shared_ptr<ColorAttachment>& attachment)
  {
    YK_ASSERT(FrameBufferStructure::CheckAttachmentCompatibility(attachment->GetAttachmentType()), "[SYSTEM/VULKAN] Invalid attachment in-place");
    YK_ASSERT(r_RenderPassData.Attachments[m_Data.Views.size()].format == attachment->GetFormat(), "[SYSTEM/VULKAN] Invalid attachment format");
    FrameBufferStructure::CheckAttachmentsExtent(attachment->GetExtent());

    m_Data.Views.push_back(attachment->GetView());
  }

  void FrameBufferStructure::AddView(const std::shared_ptr<DepthStencilAttachment>& attachment)
  {
    YK_ASSERT(FrameBufferStructure::CheckAttachmentCompatibility(attachment->GetAttachmentType()), "[SYSTEM/VULKAN] Invalid attachment in-place");
    YK_ASSERT(r_RenderPassData.Attachments[m_Data.Views.size()].format == attachment->GetFormat(), "[SYSTEM/VULKAN] Invalid attachment format");
    FrameBufferStructure::CheckAttachmentsExtent(attachment->GetExtent());

    m_Data.Views.push_back(attachment->GetView());
  }

  void FrameBufferStructure::AddView(const std::shared_ptr<ResolveAttachment>& attachment)
  {
    YK_ASSERT(FrameBufferStructure::CheckAttachmentCompatibility(attachment->GetAttachmentType()), "[SYSTEM/VULKAN] Invalid attachment in-place");
    YK_ASSERT(r_RenderPassData.Attachments[m_Data.Views.size()].format == attachment->GetFormat(), "[SYSTEM/VULKAN] Invalid attachment format");
    FrameBufferStructure::CheckAttachmentsExtent(attachment->GetExtent());

    m_Data.Views.push_back(attachment->GetView());
  }

  void FrameBufferStructure::AddView(const std::shared_ptr<InputAttachment>& attachment)
  {
    YK_ASSERT(FrameBufferStructure::CheckAttachmentCompatibility(attachment->GetAttachmentType()), "[SYSTEM/VULKAN] Invalid attachment in-place");
    YK_ASSERT(r_RenderPassData.Attachments[m_Data.Views.size()].format == attachment->GetFormat(), "[SYSTEM/VULKAN] Invalid attachment format");
    FrameBufferStructure::CheckAttachmentsExtent(attachment->GetExtent());

    m_Data.Views.push_back(attachment->GetView());
  }

  bool FrameBufferStructure::CheckAttachmentCompatibility(const AttachmentTypeFlags& type)
  {
    uint32_t counter = 0;

    if (type & YK_ATTACHMENT_TYPE_INPUT_BIT)
    {
      for (size_t i = 0; i < r_RenderPassData.Subpasses.size(); i++)
      {
        counter += r_RenderPassData.Subpasses[i].colorAttachmentCount;

        if (r_RenderPassData.Subpasses[i].pDepthStencilAttachment)
          counter++;

        counter += r_RenderPassData.ResolveAttachmentsSizes[i];

        for (size_t j = 0; j < r_RenderPassData.Subpasses[i].inputAttachmentCount; j++)
        {
          if (counter == m_Data.Views.size())
            return true;
          else
            counter++;
        }
      }
      YK_ERROR("[SYSTEM/VULKAN] Invalid input attachment or attachment out of place");
      return false;
    }
    else if (type & YK_ATTACHMENT_TYPE_COLOR_BIT)
    {
      for (size_t i = 0; i < r_RenderPassData.Subpasses.size(); i++)
      {
        for (size_t j = 0; j < r_RenderPassData.Subpasses[i].colorAttachmentCount; j++)
        {
          if (counter == m_Data.Views.size())
            return true;
          else
            counter++;
        }

        if (r_RenderPassData.Subpasses[i].pDepthStencilAttachment)
          counter++;

        counter += r_RenderPassData.ResolveAttachmentsSizes[i];

        counter += r_RenderPassData.Subpasses[i].inputAttachmentCount;
      }
      YK_ERROR("[SYSTEM/VULKAN] Invalid color attachment or attachment out of place");
      return false;
    }
    else if (type & YK_ATTACHMENT_TYPE_DEPTH_STENCIL_BIT)
    {
      for (size_t i = 0; i < r_RenderPassData.Subpasses.size(); i++)
      {
        counter += r_RenderPassData.Subpasses[i].colorAttachmentCount;

        if (r_RenderPassData.Subpasses[i].pDepthStencilAttachment)
        {
          if (counter == m_Data.Views.size())
            return true;
        }
        else
          counter++;

        counter += r_RenderPassData.ResolveAttachmentsSizes[i];

        counter += r_RenderPassData.Subpasses[i].inputAttachmentCount;
      }
      YK_ERROR("[SYSTEM/VULKAN] Invalid depth/stencil attachment or attachment out of place");
      return false;
    }
    else if (type & YK_ATTACHMENT_TYPE_RESOLVE_BIT)
    {
      for (size_t i = 0; i < r_RenderPassData.Subpasses.size(); i++)
      {
        counter += r_RenderPassData.Subpasses[i].colorAttachmentCount;

        if (r_RenderPassData.Subpasses[i].pDepthStencilAttachment)
          counter++;

        for (size_t j = 0; j < r_RenderPassData.ResolveAttachmentsSizes[i]; j++)
        {
          if (counter == m_Data.Views.size())
            return true;
          else
            counter++;
        }

        counter += r_RenderPassData.Subpasses[i].inputAttachmentCount;
      }
      YK_ERROR("[SYSTEM/VULKAN] Invalid depth/stencil attachment or attachment out of place");
      return false;
    }
    YK_ERROR("[SYSTEM/VULKAN] Invalid attachment type");
    return false;
  }

  void FrameBufferStructure::CheckAttachmentsExtent(const VkExtent3D& extent)
  {
    if (m_Data.Height.has_value() && m_Data.Width.has_value())
    {
      YK_ASSERT(m_Data.Width.value() == extent.width && m_Data.Height.value() == extent.height, "[SYSTEM/VULKAN] Invalid attachment extent");
    }
    else
    {
      m_Data.Width.emplace(extent.width);
      m_Data.Height.emplace(extent.height);
    }
  }

  FrameBuffer::~FrameBuffer()
  {
    vkDestroyFramebuffer(Renderer::GetDevice()->GetLogical(), m_Framebuffer, VK_NULL_HANDLE);
  }

  std::shared_ptr<FrameBuffer> FrameBuffer::Create(const std::shared_ptr<RenderPass>& rendepass, FrameBufferStructure& structure)
  {
    std::shared_ptr<FrameBuffer> framebuffer = std::make_shared<FrameBuffer>();

    framebuffer->r_RenderPass = rendepass;

    FrameBufferData data = structure.GetData();

    VkFramebufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.renderPass = framebuffer->r_RenderPass->Get();
    createInfo.attachmentCount = static_cast<uint32_t>(data.Views.size());
    createInfo.pAttachments = data.Views.data();
    createInfo.width = data.Width.value();
    createInfo.height = data.Height.value();
    createInfo.layers = 1;

    VkResult result = vkCreateFramebuffer(Renderer::GetDevice()->GetLogical(), &createInfo, VK_NULL_HANDLE, &framebuffer->m_Framebuffer);
    YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create a swapchain framebuffer. Error: {}", Utils::VkResultToString(result));

    return framebuffer;
  }

}