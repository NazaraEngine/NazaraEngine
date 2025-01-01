// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Renderer/WindowSwapchain.hpp>
#include <Nazara/Platform/Window.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>

namespace Nz
{
	WindowSwapchain::WindowSwapchain(std::shared_ptr<RenderDevice> renderDevice, Window& window, SwapchainParameters parameters) :
	m_renderDevice(std::move(renderDevice)),
	m_window(&window),
	m_parameters(std::move(parameters)),
	m_renderOnlyIfFocused(false)
	{
		NazaraAssertMsg(m_renderDevice, "invalid render device");

		if (m_window->IsValid())
		{
			m_isMinimized = window.IsMinimized();

			if (!m_isMinimized)
				m_swapchain = m_renderDevice->InstantiateSwapchain(window.GetHandle(), window.GetSize(), m_parameters);
		}
		else
			m_isMinimized = true; //< consider it minimized so AcquireFrame returns no frame

		ConnectSignals();
	}

	const Vector2ui& WindowSwapchain::GetSize() const
	{
		if (m_swapchain)
			return m_swapchain->GetSize();
		else if (m_window)
			return m_window->GetSize();
		else
		{
			// Window has been destroyed, return a dummy size
			static constexpr Vector2ui dummySize(1, 1);
			return dummySize;
		}
	}

	void WindowSwapchain::ConnectSignals()
	{
		WindowEventHandler& windowEvents = m_window->GetEventHandler();
		m_onCreated.Connect(windowEvents.OnCreated, [this](const WindowEventHandler* /*eventHandler*/)
		{
			m_isMinimized = m_window->IsMinimized();
			if (!m_isMinimized)
			{
				m_swapchain = m_renderDevice->InstantiateSwapchain(m_window->GetHandle(), m_window->GetSize(), m_parameters);
				OnSwapchainCreated(this, *m_swapchain);
			}
		});

		m_onDestruction.Connect(windowEvents.OnDestruction, [this](const WindowEventHandler* /*eventHandler*/)
		{
			OnSwapchainDestroy(this);
			m_swapchain.reset();
			m_isMinimized = true;
			m_window = nullptr;
		});

		m_onGainedFocus.Connect(windowEvents.OnGainedFocus, [this](const WindowEventHandler* /*eventHandler*/)
		{
			m_hasFocus = true;
		});

		m_onLostFocus.Connect(windowEvents.OnLostFocus, [this](const WindowEventHandler* /*eventHandler*/)
		{
			m_hasFocus = false;
		});

		m_onMinimized.Connect(windowEvents.OnMinimized, [this](const WindowEventHandler* /*eventHandler*/)
		{
			m_isMinimized = true;
		});

		m_onResized.Connect(windowEvents.OnResized, [this](const WindowEventHandler* /*eventHandler*/, const WindowEvent::SizeEvent& event)
		{
			m_swapchain->NotifyResize({ event.width, event.height });
		});

		m_onRestored.Connect(windowEvents.OnRestored, [this](const WindowEventHandler* /*eventHandler*/)
		{
			if (!m_swapchain)
			{
				m_swapchain = m_renderDevice->InstantiateSwapchain(m_window->GetHandle(), m_window->GetSize(), m_parameters);
				OnSwapchainCreated(this, *m_swapchain);
			}

			m_isMinimized = false;
		});
	}
}
