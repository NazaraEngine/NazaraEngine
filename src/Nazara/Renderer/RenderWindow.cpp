// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderWindow.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Thread.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	void RenderWindow::Display()
	{
		if (m_framerateLimit > 0)
		{
			int remainingTime = 1000/static_cast<int>(m_framerateLimit) - static_cast<int>(m_clock.GetMilliseconds());
			if (remainingTime > 0)
				Thread::Sleep(remainingTime);

			m_clock.Restart();
		}
	}

	void RenderWindow::EnableVerticalSync(bool enabled)
	{
		///TODO
	}

	bool RenderWindow::OnWindowCreated()
	{
		auto surface = Renderer::GetRendererImpl()->CreateRenderSurfaceImpl();
		if (!surface->Create(GetHandle()))
		{
			NazaraError("Failed to create render surface: " + Error::GetLastError());
			return false;
		}

		auto impl = Renderer::GetRendererImpl()->CreateRenderWindowImpl();
		if (!impl->Create(surface.get(), GetSize(), m_parameters))
		{
			NazaraError("Failed to create render window implementation: " + Error::GetLastError());
			return false;
		}

		m_impl = std::move(impl);
		m_surface = std::move(surface);

		m_clock.Restart();

		return true;
	}

	void RenderWindow::OnWindowDestroy()
	{
		m_impl.reset();
		m_surface.reset();
	}

	void RenderWindow::OnWindowResized()
	{
	}
}
