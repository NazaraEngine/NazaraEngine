// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/WindowSwapchain.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/Platform/Window.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	WindowSwapchain::WindowSwapchain(std::shared_ptr<RenderDevice> renderDevice, Window& window, SwapchainParameters parameters) :
	m_renderDevice(std::move(renderDevice)),
	m_window(&window),
	m_parameters(std::move(parameters)),
	m_renderOnlyIfFocused(false)
	{
		NazaraAssert(m_renderDevice, "invalid render device");

		if (m_window->IsValid())
		{
			m_swapchain = m_renderDevice->InstantiateSwapchain(window.GetHandle(), window.GetSize(), m_parameters);
			m_isMinimized = window.IsMinimized();
		}
		else
			m_isMinimized = true; //< consider it minimized so AcquireFrame returns no frame

		ConnectSignals();
	}

	void WindowSwapchain::ConnectSignals()
	{
		WindowEventHandler& windowEvents = m_window->GetEventHandler();
		m_onCreated.Connect(windowEvents.OnCreated, [this](const WindowEventHandler* /*eventHandler*/)
		{
			// Recreate swapchain
			m_swapchain = m_renderDevice->InstantiateSwapchain(m_window->GetHandle(), m_window->GetSize(), m_parameters);
			m_isMinimized = m_window->IsMinimized();
		});

		m_onDestruction.Connect(windowEvents.OnDestruction, [this](const WindowEventHandler* /*eventHandler*/)
		{
			m_swapchain.reset();
			m_isMinimized = true;
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
			m_isMinimized = false;
		});
	}
}
