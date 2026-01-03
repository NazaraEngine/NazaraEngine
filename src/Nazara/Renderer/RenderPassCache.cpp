// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Renderer/RenderPassCache.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>

namespace Nz
{
	const std::shared_ptr<RenderPass>& RenderPassCache::Get(const std::vector<RenderPass::Attachment>& attachments, const std::vector<RenderPass::SubpassDescription>& subpassDescriptions, const std::vector<RenderPass::SubpassDependency>& subpassDependencies) const
	{
		RenderPassData data;
		data.attachmentCount = attachments.size();
		data.attachments = attachments.data();

		data.dependencyCount = subpassDependencies.size();
		data.subpassDependencies = subpassDependencies.data();

		data.descriptionCount = subpassDescriptions.size();
		data.subpassDescriptions = subpassDescriptions.data();

		auto it = m_renderPasses.find(data);
		if (it != m_renderPasses.end())
			return it->second;

		std::shared_ptr<RenderPass> renderPass = m_device.InstantiateRenderPass(attachments, subpassDescriptions, subpassDependencies);

		return m_renderPasses.emplace(renderPass, renderPass).first->second;
	}

	std::size_t RenderPassCache::Hasher::operator()(const RenderPassData& renderPassData) const
	{
		std::size_t seed = 0;
		for (std::size_t i = 0; i < renderPassData.attachmentCount; ++i)
		{
			const auto& attachment = renderPassData.attachments[i];

			HashCombine(seed, attachment.format);
			HashCombine(seed, attachment.loadOp);
			HashCombine(seed, attachment.stencilLoadOp);
			HashCombine(seed, attachment.storeOp);
			HashCombine(seed, attachment.stencilStoreOp);
			HashCombine(seed, attachment.initialLayout);
			HashCombine(seed, attachment.finalLayout);
		}

		for (std::size_t i = 0; i < renderPassData.descriptionCount; ++i)
		{
			const auto& subpassDesc = renderPassData.subpassDescriptions[i];

			auto CombineAttachment = [&](const RenderPass::AttachmentReference& attachmentReference)
			{
				HashCombine(seed, attachmentReference.attachmentIndex);
				HashCombine(seed, attachmentReference.attachmentLayout);
			};

			for (const auto& colorAttachment : subpassDesc.colorAttachment)
				CombineAttachment(colorAttachment);

			for (const auto& inputAttachment : subpassDesc.inputAttachments)
				CombineAttachment(inputAttachment);

			if (subpassDesc.depthStencilAttachment)
				CombineAttachment(*subpassDesc.depthStencilAttachment);

			for (const auto& attachmentIndex : subpassDesc.preserveAttachments)
				HashCombine(seed, attachmentIndex);
		}

		for (std::size_t i = 0; i < renderPassData.dependencyCount; ++i)
		{
			const auto& subpassDep = renderPassData.subpassDependencies[i];

			HashCombine(seed, subpassDep.fromSubpassIndex);
			HashCombine(seed, subpassDep.fromStages);
			HashCombine(seed, subpassDep.fromAccessFlags);

			HashCombine(seed, subpassDep.toSubpassIndex);
			HashCombine(seed, subpassDep.toStages);
			HashCombine(seed, subpassDep.toAccessFlags);

			HashCombine(seed, subpassDep.tilable);
		}

		return seed;
	}

	bool RenderPassCache::EqualityChecker::operator()(const RenderPassData& lhs, const RenderPassData& rhs) const
	{
		if (lhs.attachmentCount != rhs.attachmentCount ||
		    lhs.dependencyCount != rhs.dependencyCount ||
			lhs.descriptionCount != rhs.descriptionCount)
			return false;

		for (std::size_t i = 0; i < lhs.attachmentCount; ++i)
		{
			const auto& lhsAttachment = lhs.attachments[i];
			const auto& rhsAttachment = rhs.attachments[i];

			if (lhsAttachment.format != rhsAttachment.format ||
			    lhsAttachment.loadOp != rhsAttachment.loadOp ||
			    lhsAttachment.stencilLoadOp != rhsAttachment.stencilLoadOp ||
			    lhsAttachment.storeOp != rhsAttachment.storeOp ||
			    lhsAttachment.stencilStoreOp != rhsAttachment.stencilStoreOp ||
			    lhsAttachment.initialLayout != rhsAttachment.initialLayout ||
			    lhsAttachment.finalLayout != rhsAttachment.finalLayout)
				return false;
		}

		for (std::size_t i = 0; i < lhs.dependencyCount; ++i)
		{
			const auto& lhsDependency = lhs.subpassDependencies[i];
			const auto& rhsDependency = rhs.subpassDependencies[i];

			if (lhsDependency.fromSubpassIndex != rhsDependency.fromSubpassIndex ||
			    lhsDependency.fromStages != rhsDependency.fromStages ||
			    lhsDependency.fromAccessFlags != rhsDependency.fromAccessFlags ||
			    lhsDependency.toSubpassIndex != rhsDependency.toSubpassIndex ||
			    lhsDependency.toStages != rhsDependency.toStages ||
			    lhsDependency.toAccessFlags != rhsDependency.toAccessFlags ||
			    lhsDependency.tilable != rhsDependency.tilable)
				return false;
		}

		for (std::size_t i = 0; i < lhs.descriptionCount; ++i)
		{
			const auto& lhsSubpassDesc = lhs.subpassDescriptions[i];
			const auto& rhsSubpassDesc = rhs.subpassDescriptions[i];

			auto CompareAttachments = [&](const RenderPass::AttachmentReference& lhsAttachmentReference, const RenderPass::AttachmentReference& rhsAttachmentReference)
			{
				if (lhsAttachmentReference.attachmentIndex != rhsAttachmentReference.attachmentIndex)
					return false;

				if (lhsAttachmentReference.attachmentLayout != rhsAttachmentReference.attachmentLayout)
					return false;

				return true;
			};

			if (std::equal(lhsSubpassDesc.colorAttachment.begin(), lhsSubpassDesc.colorAttachment.end(), rhsSubpassDesc.colorAttachment.begin(), CompareAttachments) &&
			    !std::equal(lhsSubpassDesc.inputAttachments.begin(), lhsSubpassDesc.inputAttachments.end(), rhsSubpassDesc.inputAttachments.begin(), CompareAttachments))
				return false;

			if (lhsSubpassDesc.depthStencilAttachment.has_value() != rhsSubpassDesc.depthStencilAttachment.has_value())
				return false;

			if (lhsSubpassDesc.depthStencilAttachment.has_value() && CompareAttachments(*lhsSubpassDesc.depthStencilAttachment, *rhsSubpassDesc.depthStencilAttachment))
				return false;

			if (lhsSubpassDesc.preserveAttachments != rhsSubpassDesc.preserveAttachments)
				return false;
		}

		return true;
	}
}
