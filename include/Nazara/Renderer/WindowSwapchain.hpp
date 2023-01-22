// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERWINDOW_HPP
#define NAZARA_RENDERER_RENDERWINDOW_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Platform/WindowEventHandler.hpp>
#include <Nazara/Renderer/Swapchain.hpp>
#include <Nazara/Renderer/SwapchainParameters.hpp>
#include <memory>

namespace Nz
{
	class RenderDevice;
	class Window;

	class NAZARA_RENDERER_API WindowSwapchain
	{
		public:
			WindowSwapchain(std::shared_ptr<RenderDevice> renderDevice, Window& window, SwapchainParameters parameters = SwapchainParameters());
			WindowSwapchain(const WindowSwapchain&) = delete;
			inline WindowSwapchain(WindowSwapchain&& windowSwapchain) noexcept;
			~WindowSwapchain() = default;

			inline RenderFrame AcquireFrame();

			inline bool DoesRenderOnlyIfFocused() const;

			inline void EnableRenderOnlyIfFocused(bool enable = true);

			inline Swapchain& GetSwapchain();
			inline const Swapchain& GetSwapchain() const;

			WindowSwapchain& operator=(const WindowSwapchain&) = default;
			inline WindowSwapchain& operator=(WindowSwapchain&& windowSwapchain) noexcept;

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

#endif // NAZARA_RENDERER_RENDERWINDOW_HPP
