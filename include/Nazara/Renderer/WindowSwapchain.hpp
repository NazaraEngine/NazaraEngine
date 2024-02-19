// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_WINDOWSWAPCHAIN_HPP
#define NAZARA_RENDERER_WINDOWSWAPCHAIN_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Platform/WindowEventHandler.hpp>
#include <Nazara/Renderer/Swapchain.hpp>
#include <Nazara/Renderer/SwapchainParameters.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <memory>

namespace Nz
{
	class Framebuffer;
	class RenderDevice;
	class Window;

	class NAZARA_RENDERER_API WindowSwapchain
	{
		public:
			WindowSwapchain(std::shared_ptr<RenderDevice> renderDevice, Window& window, SwapchainParameters parameters = SwapchainParameters());
			WindowSwapchain(const WindowSwapchain&) = delete;
			WindowSwapchain(WindowSwapchain&&) = delete;
			~WindowSwapchain() = default;

			inline RenderFrame AcquireFrame();

			inline bool DoesRenderOnlyIfFocused() const;

			inline void EnableRenderOnlyIfFocused(bool enable = true);

			inline const Framebuffer& GetFramebuffer(std::size_t i) const;
			inline std::size_t GetFramebufferCount() const;
			inline const RenderPass& GetRenderPass() const;
			const Vector2ui& GetSize() const;
			inline Swapchain* GetSwapchain();
			inline const Swapchain* GetSwapchain() const;
			inline RenderResources& GetTransientResources();

			WindowSwapchain& operator=(const WindowSwapchain&) = delete;
			WindowSwapchain& operator=(WindowSwapchain&& windowSwapchain) = delete;

			NazaraSignal(OnSwapchainCreated, WindowSwapchain* /*swapchain*/, Swapchain& /*swapchain*/);
			NazaraSignal(OnSwapchainDestroy, WindowSwapchain* /*swapchain*/);

		private:
			void ConnectSignals();
			inline void DisconnectSignals();

			NazaraSlot(WindowEventHandler, OnCreated, m_onCreated);
			NazaraSlot(WindowEventHandler, OnDestruction, m_onDestruction);
			NazaraSlot(WindowEventHandler, OnGainedFocus, m_onGainedFocus);
			NazaraSlot(WindowEventHandler, OnLostFocus, m_onLostFocus);
			NazaraSlot(WindowEventHandler, OnMinimized, m_onMinimized);
			NazaraSlot(WindowEventHandler, OnResized, m_onResized);
			NazaraSlot(WindowEventHandler, OnRestored, m_onRestored);

			std::shared_ptr<RenderDevice> m_renderDevice;
			std::shared_ptr<Swapchain> m_swapchain;
			MovablePtr<Window> m_window;
			SwapchainParameters m_parameters;
			bool m_renderOnlyIfFocused;
			bool m_hasFocus;
			bool m_isMinimized;
	};
}

#include <Nazara/Renderer/WindowSwapchain.inl>

#endif // NAZARA_RENDERER_WINDOWSWAPCHAIN_HPP
