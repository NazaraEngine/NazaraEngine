// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_RENDERWINDOW_HPP
#define NAZARA_GRAPHICS_RENDERWINDOW_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/RenderTarget.hpp>
#include <Nazara/Renderer/WindowSwapchain.hpp>

namespace Nz
{
	class FrameGraph;

	class NAZARA_GRAPHICS_API RenderWindow : public RenderTarget
	{
		public:
			inline RenderWindow(Swapchain& swapchain);
			RenderWindow(WindowSwapchain& windowSwapchain);
			RenderWindow(const RenderWindow&) = delete;
			RenderWindow(RenderWindow&&) = delete;
			~RenderWindow() = default;

			std::size_t OnBuildGraph(FrameGraph& graph, std::size_t attachmentIndex) const override;

			const Vector2ui& GetSize() const override;

			RenderWindow& operator=(const RenderWindow&) = delete;
			RenderWindow& operator=(RenderWindow&&) = delete;

			static constexpr Int32 DefaultRenderOrder = 1000;

		private:
			void SetSwapchain(Swapchain* swapchain);

			NazaraSlot(Swapchain, OnSwapchainResize, m_onSwapchainResize);
			NazaraSlot(WindowSwapchain, OnSwapchainCreated, m_onSwapchainCreated);
			NazaraSlot(WindowSwapchain, OnSwapchainDestroy, m_onSwapchainDestroy);

			Swapchain* m_swapchain;
			WindowSwapchain* m_windowSwapchain;
	};
}

#include <Nazara/Graphics/RenderWindow.inl>

#endif // NAZARA_GRAPHICS_RENDERWINDOW_HPP
