// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderWindow.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Renderer/Debug.hpp>

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

	inline const std::shared_ptr<RenderDevice>& RenderWindow::GetRenderDevice() const
	{
		return m_renderDevice;
	}

	inline const RenderTarget* RenderWindow::GetRenderTarget() const
	{
		return m_impl.get();
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

#include <Nazara/Renderer/DebugOff.hpp>
