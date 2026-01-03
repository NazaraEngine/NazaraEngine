// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/ErrorFlags.hpp>

namespace Nz
{
	inline RenderFrame WindowSwapchain::AcquireFrame()
	{
		if (m_isMinimized || (!m_hasFocus && m_renderOnlyIfFocused))
			return RenderFrame{};

		return m_swapchain->AcquireFrame();
	}

	inline bool WindowSwapchain::DoesRenderOnlyIfFocused() const
	{
		return m_renderOnlyIfFocused;
	}

	inline void WindowSwapchain::EnableRenderOnlyIfFocused(bool enable)
	{
		m_renderOnlyIfFocused = enable;
	}

	inline const Framebuffer& WindowSwapchain::GetFramebuffer(std::size_t i) const
	{
		assert(m_swapchain);
		return m_swapchain->GetFramebuffer(i);
	}

	inline std::size_t WindowSwapchain::GetFramebufferCount() const
	{
		assert(m_swapchain);
		return m_swapchain->GetFramebufferCount();
	}

	inline const std::shared_ptr<RenderDevice>& WindowSwapchain::GetRenderDevice() const
	{
		return m_renderDevice;
	}

	inline const RenderPass& WindowSwapchain::GetRenderPass() const
	{
		assert(m_swapchain);
		return m_swapchain->GetRenderPass();
	}

	inline Swapchain* WindowSwapchain::GetSwapchain()
	{
		return m_swapchain.get();
	}

	inline const Swapchain* WindowSwapchain::GetSwapchain() const
	{
		return m_swapchain.get();
	}

	inline RenderResources& WindowSwapchain::GetTransientResources()
	{
		return m_swapchain->GetTransientResources();
	}

	void WindowSwapchain::DisconnectSignals()
	{
		m_onGainedFocus.Disconnect();
		m_onLostFocus.Disconnect();
		m_onMinimized.Disconnect();
		m_onResized.Disconnect();
		m_onRestored.Disconnect();
	}
}
