#pragma once

#include <set>
#include <optional>
#include <vector>
#include <memory>

#include <vulkan/vulkan.h>

namespace yk
{

  struct RenderPassData
  {
    std::vector<VkAttachmentDescription> Attachments;
    std::vector<VkSubpassDescription> Subpasses;
    std::vector<VkSubpassDependency> Dependencies;
    std::vector<uint32_t> ResolveAttachmentsSizes;
  };

  struct Subpass
  {
    VkSubpassDescription Description;
    VkSubpassDependency Dependency;
    VkSampleCountFlagBits SharedSampleCount;
    std::vector<VkAttachmentReference> AllReferences;
    std::optional<VkAttachmentReference> DepthStencilAttachmentReference;
    std::vector<VkAttachmentReference> ResolveAttachmentReferences;
    std::vector<VkAttachmentReference> ColorAttachmentReferences;
    std::vector<VkAttachmentReference> InputAttachmnetReferences;
    std::optional<VkAttachmentDescription> DepthStencilAttachment;
    std::vector<VkAttachmentDescription> ResolveAttachmnets;
    std::vector<VkAttachmentDescription> ColorAttachments;
    std::vector<VkAttachmentDescription> InputAttachmnets;
    std::set<uint32_t> PreserveAttachmnets;
  };

  class RenderPassStructure
  {
  public:
    void NewSubpass(const VkSampleCountFlagBits& sample_count = VK_SAMPLE_COUNT_1_BIT);
    void AddColorAttachment(uint32_t attachment, const VkFormat& format, const VkAttachmentLoadOp& load_op, const VkAttachmentStoreOp& store_op, const VkImageLayout& initial_layout, const VkImageLayout& final_layout);
    void AddDepthStencilAttachment(uint32_t attachment, const VkFormat& format, const VkAttachmentLoadOp& load_op, const VkAttachmentStoreOp& store_op, const VkImageLayout& initial_layout, const VkImageLayout& final_layout);
    void AddDepthStencilAttachment(uint32_t attachment, const VkFormat& format, const VkAttachmentLoadOp& load_op, const VkAttachmentStoreOp& store_op, const VkAttachmentLoadOp& stencil_load_op, const VkAttachmentStoreOp& stencil_store_op, const VkImageLayout& initial_layout, const VkImageLayout& final_layout);
    void AddInputAttachment(uint32_t attachment, const VkFormat& format, const VkAttachmentLoadOp& load_op, const VkAttachmentStoreOp& store_op, const VkImageLayout& initial_layout, const VkImageLayout& final_layout);
    void AddResolveAttachment(uint32_t attachment, const VkFormat& format, const VkImageLayout& initial_layout, const VkImageLayout& final_layout);
    void PreserveAttachment(uint32_t attachment);
    void SubpassDependency(const VkPipelineStageFlags& src_stages, const VkAccessFlags& src_access, const VkPipelineStageFlags& dst_stages, const VkAccessFlags& dst_access);

    RenderPassData GetDataAndReset();

  private:
    void CheckForDuplicateAttachmentIndex(uint32_t attachment) const;

  private:
    uint32_t m_CurrentSubpass = 0;
    std::vector<Subpass> m_Subpasses;
  };

  class RenderPass
  {
  public:
    ~RenderPass();

    const VkRenderPass& Get() const { return m_RenderPass; }

    static std::shared_ptr<RenderPass> Create(const RenderPassData& data);

  private:
    VkRenderPass m_RenderPass = VK_NULL_HANDLE;
  };

}