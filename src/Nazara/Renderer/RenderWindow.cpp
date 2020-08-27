// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderWindow.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <chrono>
#include <thread>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	void RenderWindow::Display()
	{
		if (m_framerateLimit > 0)
		{
			int remainingTime = 1000 / static_cast<int>(m_framerateLimit) - static_cast<int>(m_clock.GetMilliseconds());
			if (remainingTime > 0)
				std::this_thread::sleep_for(std::chrono::milliseconds(remainingTime));

			m_clock.Restart();
		}
	}

	void RenderWindow::EnableVerticalSync(bool enabled)
	{
		///TODO
	}

	std::shared_ptr<RenderDevice> RenderWindow::GetRenderDevice()
	{
		if (!m_impl)
			return std::shared_ptr<RenderDevice>();

		return m_impl->GetRenderDevice();
	}

	bool RenderWindow::OnWindowCreated()
	{
		RendererImpl *rendererImpl = Renderer::GetRendererImpl();
		auto surface = rendererImpl->CreateRenderSurfaceImpl();
		if (!surface->Create(GetSystemHandle()))
		{
			NazaraError("Failed to create render surface: " + Error::GetLastError());
			return false;
		}

		auto impl = rendererImpl->CreateRenderWindowImpl(*this);
		if (!impl->Create(rendererImpl, surface.get(), m_parameters))
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
