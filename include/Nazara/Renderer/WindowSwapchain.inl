// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline WindowSwapchain::~WindowSwapchain()
	{
		OnRenderTargetRelease(this);
	}

	inline RenderFrame WindowSwapchain::AcquireFrame()
	{
		if (m_isMinimized || (!m_hasFocus && m_renderOnlyIfFocused))
			return RenderFrame{};

		return m_swapchain->AcquireFrame();
	}

	inline void WindowSwapchain::BlitTexture(RenderFrame& renderFrame, CommandBufferBuilder& builder, const Texture& texture) const
	{
		return m_swapchain->BlitTexture(renderFrame, builder, texture);
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

	inline const RenderPass& WindowSwapchain::GetRenderPass() const
	{
		assert(m_swapchain);
		return m_swapchain->GetRenderPass();
	}

	inline Swapchain& WindowSwapchain::GetSwapchain()
	{
		return *m_swapchain;
	}

	inline const Swapchain& WindowSwapchain::GetSwapchain() const
	{
		return *m_swapchain;
	}

	inline TransientResources& WindowSwapchain::Transient()
	{
		return m_swapchain->Transient();
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

#include <Nazara/Renderer/DebugOff.hpp>
