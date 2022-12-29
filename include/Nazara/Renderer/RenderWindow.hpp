// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERWINDOW_HPP
#define NAZARA_RENDERER_RENDERWINDOW_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Platform/Window.hpp>
#include <Nazara/Renderer/RenderSurface.hpp>
#include <Nazara/Renderer/RenderWindowImpl.hpp>
#include <Nazara/Renderer/RenderWindowParameters.hpp>
#include <memory>

namespace Nz
{
	class RenderDevice;

	class NAZARA_RENDERER_API RenderWindow : public Window
	{
		public:
			inline RenderWindow();
			inline RenderWindow(std::shared_ptr<RenderDevice> renderDevice, VideoMode mode, const std::string& title, WindowStyleFlags style = WindowStyle_Default, const RenderWindowParameters& parameters = RenderWindowParameters());
			inline RenderWindow(std::shared_ptr<RenderDevice> renderDevice, void* handle, const RenderWindowParameters& parameters = RenderWindowParameters());
			inline ~RenderWindow();

			RenderFrame AcquireFrame();

			bool Create(std::shared_ptr<RenderDevice> renderDevice, VideoMode mode, const std::string& title, WindowStyleFlags style = WindowStyle_Default, const RenderWindowParameters& parameters = RenderWindowParameters());
			bool Create(std::shared_ptr<RenderDevice> renderDevice, void* handle, const RenderWindowParameters &parameters = RenderWindowParameters());

			void EnableVerticalSync(bool enabled);

			inline const std::shared_ptr<RenderDevice>& GetRenderDevice() const;
			inline const RenderTarget* GetRenderTarget() const;
			inline RenderSurface* GetSurface();

			inline bool IsValid() const;

			inline void SetFramerateLimit(unsigned int limit);

			RenderWindow &operator=(const RenderWindow &) = delete;
			RenderWindow &operator=(RenderWindow &&) = delete; ///TODO

		protected:
			bool OnWindowCreated() override;
			void OnWindowDestroy() override;
			void OnWindowResized() override;

		private:
			std::shared_ptr<RenderDevice> m_renderDevice;
			std::unique_ptr<RenderSurface> m_surface;
			std::unique_ptr<RenderWindowImpl> m_impl;
			MillisecondClock m_clock;
			RenderWindowParameters m_parameters;
			unsigned int m_framerateLimit;
		};
} // namespace Nz

#include <Nazara/Renderer/RenderWindow.inl>

#endif // NAZARA_RENDERER_RENDERWINDOW_HPP
