// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderPass.hpp>
#include <cassert>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline RenderPass::RenderPass(std::vector<Attachment> attachments, std::vector<SubpassDescription> subpassDescriptions, std::vector<SubpassDependency> subpassDependencies) :
	m_attachments(std::move(attachments)),
	m_subpassDependencies(std::move(subpassDependencies)),
	m_subpassDescriptions(std::move(subpassDescriptions))
	{
	}

	inline auto RenderPass::GetAttachment(std::size_t attachmentIndex) const -> const Attachment&
	{
		assert(attachmentIndex < m_attachments.size());
		return m_attachments[attachmentIndex];
	}

	inline std::size_t RenderPass::GetAttachmentCount() const
	{
		return m_attachments.size();
	}

	inline auto RenderPass::GetAttachments() const -> const std::vector<Attachment>&
	{
		return m_attachments;
	}

	inline auto RenderPass::GetSubpassDescriptions() const -> const std::vector<SubpassDescription>&
	{
		return m_subpassDescriptions;
	}

	inline auto RenderPass::GetSubpassDependencies() const -> const std::vector<SubpassDependency>&
	{
		return m_subpassDependencies;
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
