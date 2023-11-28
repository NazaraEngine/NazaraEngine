// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

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

	inline std::size_t FrameGraph::AddAttachmentArray(FramePassAttachment attachment, unsigned int layerCount)
	{
		AttachmentArray attachmentArray{ std::move(attachment) };
		attachmentArray.layerCount = layerCount;

		std::size_t id = m_attachments.size();
		m_attachments.emplace_back(std::move(attachmentArray));

		return id;
	}

	inline std::size_t FrameGraph::AddAttachmentArrayLayer(std::size_t attachmentId, std::size_t layerIndex)
	{
		attachmentId = ResolveAttachmentIndex(attachmentId);

		assert(std::holds_alternative<AttachmentArray>(m_attachments[attachmentId]));
		assert(layerIndex < std::get<AttachmentArray>(m_attachments[attachmentId]).layerCount);

		std::size_t id = m_attachments.size();
		m_attachments.emplace_back(AttachmentLayer{
			attachmentId,
			layerIndex
		});

		return id;
	}

	inline std::size_t FrameGraph::AddAttachmentCube(FramePassAttachment attachment)
	{
		std::size_t id = m_attachments.size();
		m_attachments.emplace_back(AttachmentCube{ std::move(attachment) });

		return id;
	}

	inline std::size_t FrameGraph::AddAttachmentCubeFace(std::size_t attachmentId, CubemapFace face)
	{
		attachmentId = ResolveAttachmentIndex(attachmentId);

		assert(std::holds_alternative<AttachmentCube>(m_attachments[attachmentId]));

		std::size_t id = m_attachments.size();
		m_attachments.emplace_back(AttachmentLayer{
			attachmentId,
			SafeCast<std::size_t>(face)
		});

		return id;
	}

	inline std::size_t FrameGraph::AddAttachmentProxy(std::string name, std::size_t attachmentId)
	{
		assert(attachmentId < m_attachments.size());

		std::size_t id = m_attachments.size();
		m_attachments.emplace_back(AttachmentProxy {
			attachmentId,
			std::move(name)
		});

		return id;
	}

	inline std::size_t FrameGraph::AddDummyAttachment()
	{
		std::size_t id = m_attachments.size();
		m_attachments.emplace_back(DummyAttachment{});

		return id;
	}

	inline FramePass& FrameGraph::AddPass(std::string name)
	{
		std::size_t id = m_framePasses.size();
		return m_framePasses.emplace_back(*this, id, std::move(name));
	}

	inline void FrameGraph::AddOutput(std::size_t attachmentIndex)
	{
		m_graphOutputs.push_back(attachmentIndex);
	}

	inline void FrameGraph::BindExternalTexture(std::size_t attachmentIndex, std::shared_ptr<Texture> texture)
	{
		m_externalTextures[attachmentIndex] = std::move(texture);
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
