// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanRenderPass.hpp>
#include <Nazara/VulkanRenderer/Utils.hpp>
#include <NazaraUtils/StackVector.hpp>
#include <stdexcept>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	VulkanRenderPass::VulkanRenderPass(Vk::Device& device, std::vector<Attachment> attachments, std::vector<SubpassDescription> subpassDescriptions, std::vector<SubpassDependency> subpassDependencies) :
	RenderPass(std::move(attachments), std::move(subpassDescriptions), std::move(subpassDependencies))
	{
		std::size_t totalAttachmentReference = 0;
		for (const SubpassDescription& subpassInfo : m_subpassDescriptions)
		{
			totalAttachmentReference += subpassInfo.colorAttachment.size();
			totalAttachmentReference += subpassInfo.inputAttachments.size();

			if (subpassInfo.depthStencilAttachment)
				totalAttachmentReference++;
		}

		StackVector<VkAttachmentDescription> vkAttachments = NazaraStackVector(VkAttachmentDescription, m_attachments.size());
		for (const Attachment& attachmentInfo : m_attachments)
		{
			vkAttachments.push_back({
				0,
				ToVulkan(attachmentInfo.format),
				VK_SAMPLE_COUNT_1_BIT,
				ToVulkan(attachmentInfo.loadOp),
				ToVulkan(attachmentInfo.storeOp),
				ToVulkan(attachmentInfo.stencilLoadOp),
				ToVulkan(attachmentInfo.stencilStoreOp),
				ToVulkan(attachmentInfo.initialLayout),
				ToVulkan(attachmentInfo.finalLayout)
			});
		}

		StackVector<VkAttachmentReference> vkAttachmentReferences = NazaraStackVector(VkAttachmentReference, totalAttachmentReference);
		StackVector<UInt32> vkPreserveAttachments = NazaraStackVector(UInt32, attachments.size());

		StackVector<VkSubpassDescription> vkSubpassDescs = NazaraStackVector(VkSubpassDescription, m_subpassDescriptions.size());
		for (const SubpassDescription& subpassInfo : m_subpassDescriptions)
		{
			std::size_t colorAttachmentIndex = vkAttachmentReferences.size();
			for (const AttachmentReference& attachmentRef : subpassInfo.colorAttachment)
			{
				vkAttachmentReferences.push_back({
					UInt32(attachmentRef.attachmentIndex),
					ToVulkan(attachmentRef.attachmentLayout)
				});
			}

			std::size_t inputAttachmentIndex = vkAttachmentReferences.size();
			for (const AttachmentReference& attachmentRef : subpassInfo.inputAttachments)
			{
				vkAttachmentReferences.push_back({
					UInt32(attachmentRef.attachmentIndex),
					ToVulkan(attachmentRef.attachmentLayout)
				});
			}

			std::size_t depthStencilAttachmentIndex = vkAttachmentReferences.size();
			if (subpassInfo.depthStencilAttachment)
			{
				auto& depthStencilRef = *subpassInfo.depthStencilAttachment;
				vkAttachmentReferences.push_back({
					UInt32(depthStencilRef.attachmentIndex),
					ToVulkan(depthStencilRef.attachmentLayout)
				});
			}

			assert(subpassInfo.preserveAttachments.size() <= vkPreserveAttachments.size());
			for (std::size_t attachmentIndex : subpassInfo.preserveAttachments)
				vkPreserveAttachments.push_back(UInt32(attachmentIndex));

			vkSubpassDescs.push_back({
				VkSubpassDescriptionFlags(0),
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				UInt32(subpassInfo.inputAttachments.size()),
				(!subpassInfo.inputAttachments.empty()) ? &vkAttachmentReferences[inputAttachmentIndex] : nullptr,
				UInt32(subpassInfo.colorAttachment.size()),
				(!subpassInfo.colorAttachment.empty()) ? &vkAttachmentReferences[colorAttachmentIndex] : nullptr,
				nullptr,
				(subpassInfo.depthStencilAttachment) ? &vkAttachmentReferences[depthStencilAttachmentIndex] : nullptr,
				UInt32(vkPreserveAttachments.size()),
				(!vkPreserveAttachments.empty()) ? &vkPreserveAttachments[0] : nullptr
			});
		}

		StackVector<VkSubpassDependency> vkSubpassDeps = NazaraStackVector(VkSubpassDependency, m_subpassDependencies.size());
		for (const SubpassDependency& subpassDependency : m_subpassDependencies)
		{
			auto ToSubPassIndex = [](std::size_t subpassIndex) -> UInt32
			{
				if (subpassIndex == ExternalSubpassIndex)
					return VK_SUBPASS_EXTERNAL;

				return UInt32(subpassIndex);
			};

			vkSubpassDeps.push_back({
				ToSubPassIndex(subpassDependency.fromSubpassIndex),
				ToSubPassIndex(subpassDependency.toSubpassIndex),
				ToVulkan(subpassDependency.fromStages),
				ToVulkan(subpassDependency.toStages),
				ToVulkan(subpassDependency.fromAccessFlags),
				ToVulkan(subpassDependency.toAccessFlags),
				VkDependencyFlags((subpassDependency.tilable) ? VK_DEPENDENCY_BY_REGION_BIT : 0)
			});
		}

		VkRenderPassCreateInfo renderPassInfo = {
			VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO, // sType
			nullptr,                                   // pNext
			0,                                         // flags
			UInt32(vkAttachments.size()),              // attachmentCount
			vkAttachments.data(),                      // pAttachments
			UInt32(vkSubpassDescs.size()),             // subpassCount
			vkSubpassDescs.data(),                     // pSubpasses
			UInt32(vkSubpassDeps.size()),              // dependencyCount
			vkSubpassDeps.data()                       // pDependencies
		};

		if (!m_renderPass.Create(device, renderPassInfo))
			throw std::runtime_error("failed to instantiate Vulkan render pass: " + TranslateVulkanError(m_renderPass.GetLastErrorCode()));
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		OnRenderPassRelease(this);
	}

	void VulkanRenderPass::UpdateDebugName(std::string_view name)
	{
		return m_renderPass.SetDebugName(name);
	}
}
