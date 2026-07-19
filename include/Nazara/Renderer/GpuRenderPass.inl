// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <cassert>

namespace Nz
{
	inline GpuRenderPass::GpuRenderPass(std::vector<Attachment> attachments, std::vector<SubpassDescription> subpassDescriptions, std::vector<SubpassDependency> subpassDependencies) :
	m_attachments(std::move(attachments)),
	m_subpassDependencies(std::move(subpassDependencies)),
	m_subpassDescriptions(std::move(subpassDescriptions))
	{
	}

	inline auto GpuRenderPass::GetAttachment(std::size_t attachmentIndex) const -> const Attachment&
	{
		assert(attachmentIndex < m_attachments.size());
		return m_attachments[attachmentIndex];
	}

	inline std::size_t GpuRenderPass::GetAttachmentCount() const
	{
		return m_attachments.size();
	}

	inline auto GpuRenderPass::GetAttachments() const -> const std::vector<Attachment>&
	{
		return m_attachments;
	}

	inline auto GpuRenderPass::GetSubpassDescriptions() const -> const std::vector<SubpassDescription>&
	{
		return m_subpassDescriptions;
	}

	inline auto GpuRenderPass::GetSubpassDependencies() const -> const std::vector<SubpassDependency>&
	{
		return m_subpassDependencies;
	}
}
