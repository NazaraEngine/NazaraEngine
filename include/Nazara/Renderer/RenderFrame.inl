// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderFrame.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline RenderFrame::RenderFrame() :
	RenderFrame(nullptr, false)
	{
	}

	inline RenderFrame::RenderFrame(RenderImage* renderImage, bool framebufferInvalidation) :
	m_image(renderImage),
	m_framebufferInvalidation(framebufferInvalidation)
	{
	}

	inline bool RenderFrame::IsFramebufferInvalidated() const
	{
		return m_framebufferInvalidation;
	}

	inline RenderFrame::operator bool()
	{
		return m_image != nullptr;
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
