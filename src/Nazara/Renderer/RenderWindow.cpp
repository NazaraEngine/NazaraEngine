// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderWindow.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Thread.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	RenderWindow::~RenderWindow()
	{
		// Nécessaire si Window::Destroy est appelé par son destructeur
		OnWindowDestroy();
	}

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
		auto impl = Renderer::GetRendererImpl()->CreateRenderWindowImpl();
		if (!impl->Create(GetHandle(), Vector2ui(GetWidth(), GetHeight()), m_parameters))
		{
			NazaraError("Failed to create render window implementation: " + Error::GetLastError());
			return false;
		}

		m_impl = std::move(impl);

		m_clock.Restart();

		return true;
	}

	void RenderWindow::OnWindowDestroy()
	{
		m_impl.reset();
	}

	void RenderWindow::OnWindowResized()
	{
	}
}
