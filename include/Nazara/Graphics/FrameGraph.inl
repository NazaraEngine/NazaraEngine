// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/FrameGraph.hpp>
#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline std::size_t FrameGraph::AddAttachment(FramePassAttachment attachment)
	{
		std::size_t id = m_attachments.size();
		m_attachments.emplace_back(std::move(attachment));

		return id;
	}

	inline std::size_t FrameGraph::AddAttachmentProxy(std::string name, std::size_t attachmentId)
	{
		assert(attachmentId < m_attachments.size());
		assert(std::holds_alternative<FramePassAttachment>(m_attachments[attachmentId]));

		std::size_t id = m_attachments.size();
		m_attachments.emplace_back(AttachmentProxy {
			attachmentId,
			std::move(name)
		});

		return id;
	}

	inline void FrameGraph::AddBackbufferOutput(std::size_t backbufferOutput)
	{
		m_backbufferOutputs.push_back(backbufferOutput);
	}

	inline FramePass& FrameGraph::AddPass(std::string name)
	{
		std::size_t id = m_framePasses.size();
		return m_framePasses.emplace_back(*this, id, std::move(name));
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
