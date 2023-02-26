// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/WindowSwapchain.hpp>
#include <Nazara/Platform/Window.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
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
			m_isMinimized = window.IsMinimized();

			if (!m_isMinimized)
				m_swapchain = m_renderDevice->InstantiateSwapchain(window.GetHandle(), window.GetSize(), m_parameters);
		}
		else
			m_isMinimized = true; //< consider it minimized so AcquireFrame returns no frame

		ConnectSignals();
	}

	const Framebuffer& WindowSwapchain::GetFramebuffer(std::size_t i) const
	{
		return m_swapchain->GetFramebuffer(i);
	}

	std::size_t WindowSwapchain::GetFramebufferCount() const
	{
		return m_swapchain->GetFramebufferCount();
	}

	const RenderPass& WindowSwapchain::GetRenderPass() const
	{
		return m_swapchain->GetRenderPass();
	}

	const Vector2ui& WindowSwapchain::GetSize() const
	{
		return (m_swapchain) ? m_swapchain->GetSize() : m_window->GetSize();
	}

	void WindowSwapchain::ConnectSignals()
	{
		WindowEventHandler& windowEvents = m_window->GetEventHandler();
		m_onCreated.Connect(windowEvents.OnCreated, [this](const WindowEventHandler* /*eventHandler*/)
		{
			m_isMinimized = m_window->IsMinimized();
			if (!m_isMinimized)
				m_swapchain = m_renderDevice->InstantiateSwapchain(m_window->GetHandle(), m_window->GetSize(), m_parameters);
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
			OnRenderTargetSizeChange(this, m_swapchain->GetSize());
		});

		m_onRestored.Connect(windowEvents.OnRestored, [this](const WindowEventHandler* /*eventHandler*/)
		{
			if (!m_swapchain)
				m_swapchain = m_renderDevice->InstantiateSwapchain(m_window->GetHandle(), m_window->GetSize(), m_parameters);

			m_isMinimized = false;
		});
	}
}
