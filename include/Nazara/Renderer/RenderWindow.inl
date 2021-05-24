// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderWindow.hpp>
#include <Nazara/Core/ErrorFlags.hpp>

namespace Nz
{
	inline RenderWindow::RenderWindow() :
	m_framerateLimit(0U)
	{
	}

	inline RenderWindow::RenderWindow(std::shared_ptr<RenderDevice> renderDevice, VideoMode mode, const std::string& title, WindowStyleFlags style, const RenderWindowParameters& parameters) :
	RenderWindow()
	{
		ErrorFlags errFlags(ErrorMode::ThrowException, true);

		Create(std::move(renderDevice), mode, title, style, parameters);
	}

	inline RenderWindow::RenderWindow(std::shared_ptr<RenderDevice> renderDevice, void* handle, const RenderWindowParameters& parameters)
	{
		ErrorFlags errFlags(ErrorMode::ThrowException, true);

		Create(std::move(renderDevice), handle, parameters);
	}

	inline RenderWindow::~RenderWindow()
	{
		Destroy();
	}

	inline RenderWindowImpl* RenderWindow::GetImpl()
	{
		return m_impl.get();
	}

	inline const std::shared_ptr<RenderDevice>& RenderWindow::GetRenderDevice() const
	{
		return m_renderDevice;
	}

	inline RenderSurface* RenderWindow::GetSurface()
	{
		return m_surface.get();
	}

	inline bool RenderWindow::IsValid() const
	{
		return m_impl != nullptr;
	}

	inline void RenderWindow::SetFramerateLimit(unsigned int limit)
	{
		m_framerateLimit = limit;
	}
}
