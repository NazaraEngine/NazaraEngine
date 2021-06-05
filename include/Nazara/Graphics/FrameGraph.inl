// Copyright (C) 2017 Jérôme Leclercq
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

	inline FramePass& FrameGraph::AddPass(std::string name)
	{
		std::size_t id = m_framePasses.size();
		return m_framePasses.emplace_back(*this, id, std::move(name));
	}

	inline void FrameGraph::SetBackbufferOutput(std::size_t backbufferOutput)
	{
		m_backbufferOutput = backbufferOutput;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
