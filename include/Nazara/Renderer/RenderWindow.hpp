// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERWINDOW_HPP
#define NAZARA_RENDERWINDOW_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Renderer/RenderSurface.hpp>
#include <Nazara/Renderer/RenderWindowImpl.hpp>
#include <Nazara/Renderer/RenderWindowParameters.hpp>
#include <Nazara/Utility/Window.hpp>
#include <memory>

namespace Nz
{
	class NAZARA_RENDERER_API RenderWindow : public Window
	{
		public:
			inline RenderWindow();
			inline RenderWindow(VideoMode mode, const String& title, WindowStyleFlags style = WindowStyle_Default, const RenderWindowParameters& parameters = RenderWindowParameters());
			inline RenderWindow(WindowHandle handle, const RenderWindowParameters& parameters = RenderWindowParameters());

			RenderWindow(const RenderWindow&) = delete;
			RenderWindow(RenderWindow&&) = delete; ///TODO
			virtual ~RenderWindow();

			inline bool Create(VideoMode mode, const String& title, WindowStyleFlags style = WindowStyle_Default, const RenderWindowParameters& parameters = RenderWindowParameters());
			inline bool Create(WindowHandle handle, const RenderWindowParameters& parameters = RenderWindowParameters());

			void Display();

			void EnableVerticalSync(bool enabled);

			inline RenderWindowImpl* GetImpl();

			inline bool IsValid() const;

			inline void SetFramerateLimit(unsigned int limit);

			RenderWindow& operator=(const RenderWindow&) = delete;
			RenderWindow& operator=(RenderWindow&&) = delete; ///TODO

		protected:
			bool OnWindowCreated() override;
			void OnWindowDestroy() override;
			void OnWindowResized() override;

		private:
			std::unique_ptr<RenderWindowImpl> m_impl;
			Clock m_clock;
			std::unique_ptr<RenderSurface> m_surface;
			RenderWindowParameters m_parameters;
			unsigned int m_framerateLimit;
	};
}

#include <Nazara/Renderer/RenderWindow.inl>

#endif // NAZARA_RENDERWINDOW_HPP
