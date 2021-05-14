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

	template<typename T>
	void RenderFrame::PushForRelease(T&& value)
	{
		return PushReleaseCallback([v = std::forward<T>(value)] {});
	}

	template<typename F>
	void RenderFrame::PushReleaseCallback(F&& releaseCallback)
	{
		if (!m_image)
			throw std::runtime_error("frame is either invalid or has already been presented");

		m_image->PushReleaseCallback(std::forward<F>(releaseCallback));
	}

	inline RenderFrame::operator bool()
	{
		return m_image != nullptr;
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
