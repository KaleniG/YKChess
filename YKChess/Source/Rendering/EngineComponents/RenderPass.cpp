#include "Rendering/EngineComponents/RenderPass.h"
#include "Rendering/Renderer.h"
#include "Rendering/Utils.h"

namespace yk
{

  void RenderPassStructure::NewSubpass(const VkSampleCountFlagBits& sample_count)
  {
    VkSubpassDescription description = {};
    description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    description.pPreserveAttachments;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = m_CurrentSubpass == 0 ? VK_SUBPASS_EXTERNAL : m_CurrentSubpass;
    dependency.dstSubpass = m_CurrentSubpass == 0 ? 0 : m_CurrentSubpass + 1;
    dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    Subpass subpass;
    subpass.Description = description;
    subpass.SharedSampleCount = sample_count;
    subpass.Dependency = dependency;

    m_CurrentSubpass = m_Subpasses.size();
    m_Subpasses.push_back(subpass);
  }

  void RenderPassStructure::AddColorAttachment(uint32_t attachment, const VkFormat& format, const VkAttachmentLoadOp& load_op, const VkAttachmentStoreOp& store_op, const VkImageLayout& initial_layout, const VkImageLayout & final_layout)
  {
    RenderPassStructure::CheckForDuplicateAttachmentIndex(attachment);

    VkAttachmentDescription description = {};
    description.format = format;
    description.samples = m_Subpasses[m_CurrentSubpass].SharedSampleCount;
    description.loadOp = load_op;
    description.storeOp = store_op;
    description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    description.initialLayout = initial_layout;
    description.finalLayout = final_layout;

    VkAttachmentReference reference = {};
    reference.attachment = attachment;
    reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    m_Subpasses[m_CurrentSubpass].ColorAttachments.push_back(description);
    m_Subpasses[m_CurrentSubpass].ColorAttachmentReferences.push_back(reference);
    m_Subpasses[m_CurrentSubpass].AllReferences.push_back(reference);
  }

  void RenderPassStructure::AddDepthStencilAttachment(uint32_t attachment, const VkFormat& format, const VkAttachmentLoadOp& load_op, const VkAttachmentStoreOp& store_op, const VkImageLayout& initial_layout, const VkImageLayout& final_layout)
  {
    RenderPassStructure::AddDepthStencilAttachment(attachment, format, load_op, store_op, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, initial_layout, final_layout);
  }

  void RenderPassStructure::AddDepthStencilAttachment(uint32_t attachment, const VkFormat& format, const VkAttachmentLoadOp& load_op, const VkAttachmentStoreOp& store_op, const VkAttachmentLoadOp& stencil_load_op, const VkAttachmentStoreOp& stencil_store_op, const VkImageLayout& initial_layout, const VkImageLayout& final_layout)
  {
    RenderPassStructure::CheckForDuplicateAttachmentIndex(attachment);

    YK_ASSERT(!m_Subpasses[m_CurrentSubpass].DepthStencilAttachment.has_value(), "[SYSTEM] Cannot have more than one depth/stencil attachment in a subpass");

    VkAttachmentDescription description = {};
    description.format = format;
    description.samples = m_Subpasses[m_CurrentSubpass].SharedSampleCount;
    description.loadOp = load_op;
    description.storeOp = store_op;
    description.stencilLoadOp = stencil_load_op;
    description.stencilStoreOp = stencil_store_op;
    description.initialLayout = initial_layout;
    description.finalLayout = final_layout;

    VkAttachmentReference reference = {};
    reference.attachment = attachment;
    reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    m_Subpasses[m_CurrentSubpass].DepthStencilAttachment.emplace(description);
    m_Subpasses[m_CurrentSubpass].DepthStencilAttachmentReference.emplace(reference);
    m_Subpasses[m_CurrentSubpass].AllReferences.push_back(reference);
  }

  void RenderPassStructure::AddInputAttachment(uint32_t attachment, const VkFormat& format, const VkAttachmentLoadOp& load_op, const VkAttachmentStoreOp& store_op, const VkImageLayout& initial_layout, const VkImageLayout& final_layout)
  {
    RenderPassStructure::CheckForDuplicateAttachmentIndex(attachment);

    YK_ASSERT(m_CurrentSubpass > 0, "[SYSTEM/VULKAN] Cannot use an input attachment in the first subpass");

    VkAttachmentDescription description = {};
    description.format = format;
    description.samples = m_Subpasses[m_CurrentSubpass].SharedSampleCount;
    description.loadOp = load_op;
    description.storeOp = store_op;
    description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    description.initialLayout = initial_layout;
    description.finalLayout = final_layout;

    VkAttachmentReference reference = {};
    reference.attachment = attachment;
    reference.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    m_Subpasses[m_CurrentSubpass].InputAttachmnets.push_back(description);
    m_Subpasses[m_CurrentSubpass].InputAttachmnetReferences.push_back(reference);
    m_Subpasses[m_CurrentSubpass].AllReferences.push_back(reference);
  }

  void RenderPassStructure::AddResolveAttachment(uint32_t attachment, const VkFormat& format, const VkImageLayout& initial_layout, const VkImageLayout& final_layout)
  {
    RenderPassStructure::CheckForDuplicateAttachmentIndex(attachment);

    VkAttachmentDescription description = {};
    description.format = format;
    description.samples = VK_SAMPLE_COUNT_1_BIT;
    description.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    description.initialLayout = initial_layout;
    description.finalLayout = final_layout;

    VkAttachmentReference reference = {};
    reference.attachment = attachment;
    reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    m_Subpasses[m_CurrentSubpass].ResolveAttachmnets.push_back(description);
    m_Subpasses[m_CurrentSubpass].ResolveAttachmentReferences.push_back(reference);
    m_Subpasses[m_CurrentSubpass].AllReferences.push_back(reference);
  }


  void RenderPassStructure::PreserveAttachment(uint32_t attachment)
  {
    m_Subpasses[m_CurrentSubpass].PreserveAttachmnets.emplace(attachment);
  }

  void RenderPassStructure::SubpassDependency(const VkPipelineStageFlags& src_stages, const VkAccessFlags& src_access, const VkPipelineStageFlags& dst_stages, const VkAccessFlags& dst_access)
  {
    m_Subpasses[m_CurrentSubpass].Dependency.srcStageMask |= src_stages;
    m_Subpasses[m_CurrentSubpass].Dependency.srcAccessMask |= src_access;
    m_Subpasses[m_CurrentSubpass].Dependency.dstStageMask |= dst_stages;
    m_Subpasses[m_CurrentSubpass].Dependency.dstAccessMask |= dst_access;
  }

  RenderPassData RenderPassStructure::GetDataAndReset()
  {
    YK_ASSERT(!m_Subpasses.empty(), "[SYSTEM] Cannot create a RenderPass without any Subpass");

    RenderPassData data;
    data.Subpasses.reserve(m_Subpasses.size());
    data.Dependencies.reserve(m_Subpasses.size());
    data.ResolveAttachmentsSizes.reserve(m_Subpasses.size());

    for (Subpass& subpass : m_Subpasses)
    {
      YK_ASSERT(subpass.Dependency.dstStageMask && subpass.Dependency.dstAccessMask, "[SYSTEM/VULKAN] Unspecified dependency for the subpass");

      subpass.Description.colorAttachmentCount = subpass.ColorAttachmentReferences.size();
      subpass.Description.pColorAttachments = subpass.ColorAttachmentReferences.data();
      subpass.Description.pDepthStencilAttachment = &subpass.DepthStencilAttachmentReference.value();
      subpass.Description.pResolveAttachments = subpass.ResolveAttachmentReferences.data();
      subpass.Description.inputAttachmentCount = subpass.InputAttachmnetReferences.size();
      subpass.Description.pInputAttachments = subpass.InputAttachmnetReferences.data();
      subpass.Description.preserveAttachmentCount = subpass.PreserveAttachmnets.size();

      std::vector<uint32_t> preserveAttachments(subpass.PreserveAttachmnets.begin(), subpass.PreserveAttachmnets.end());
      subpass.Description.pPreserveAttachments = preserveAttachments.data();

      for (VkAttachmentDescription& description : subpass.ColorAttachments)
        data.Attachments.push_back(description);

      if (subpass.DepthStencilAttachment.has_value())
        data.Attachments.push_back(subpass.DepthStencilAttachment.value());

      for (VkAttachmentDescription& description : subpass.ResolveAttachmnets)
        data.Attachments.push_back(description);

      for (VkAttachmentDescription& description : subpass.InputAttachmnets)
        data.Attachments.push_back(description);

      data.Subpasses.push_back(subpass.Description);
      data.Dependencies.push_back(subpass.Dependency);
      data.ResolveAttachmentsSizes.push_back(subpass.ResolveAttachmnets.size());
    }

    return data;
  }

  void RenderPassStructure::CheckForDuplicateAttachmentIndex(uint32_t attachment) const
  {
    for (const VkAttachmentReference& reference : m_Subpasses[m_CurrentSubpass].AllReferences)
      YK_ASSERT(reference.attachment != attachment, "[SYSTEM/VULKAN] Cannot use the same attachment index for multiple attachments");
  }

  RenderPass::~RenderPass()
  {
    vkDestroyRenderPass(Renderer::GetDevice()->GetLogical(), m_RenderPass, VK_NULL_HANDLE);
  }

  std::shared_ptr<RenderPass> RenderPass::Create(const RenderPassData& data)
  {
    std::shared_ptr<RenderPass> renderPass = std::make_shared<RenderPass>();

    VkRenderPassCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = static_cast<uint32_t>(data.Attachments.size());
    createInfo.pAttachments = data.Attachments.data();
    createInfo.subpassCount = static_cast<uint32_t>(data.Subpasses.size());
    createInfo.pSubpasses = data.Subpasses.data();
    createInfo.dependencyCount = static_cast<uint32_t>(data.Dependencies.size());
    createInfo.pDependencies = data.Dependencies.data();

    VkResult result = vkCreateRenderPass(Renderer::GetDevice()->GetLogical(), &createInfo, VK_NULL_HANDLE, &renderPass->m_RenderPass);
    YK_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the render pass. {}", Utils::VkResultToString(result));

    return renderPass;
  }

}