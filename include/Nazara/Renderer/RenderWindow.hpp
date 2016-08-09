// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Interface inspirée de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_RENDERWINDOW_HPP
#define NAZARA_RENDERWINDOW_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/ContextParameters.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Utility/Window.hpp>
#include <vector>

namespace Nz
{
	class AbstractImage;
	class Context;
	class Texture;
	struct ContextParameters;

	class NAZARA_RENDERER_API RenderWindow : public RenderTarget, public Window
	{
		public:
			RenderWindow() = default;
			RenderWindow(VideoMode mode, const String& title, UInt32 style = WindowStyle_Default, const ContextParameters& parameters = ContextParameters());
			RenderWindow(WindowHandle handle, const ContextParameters& parameters = ContextParameters());
			RenderWindow(const RenderWindow&) = delete;
			RenderWindow(RenderWindow&&) = delete; ///TODO
			virtual ~RenderWindow();

			bool CopyToImage(AbstractImage* image, const Vector3ui& dstPos = Vector3ui(0U)) const;
			bool CopyToImage(AbstractImage* image, const Rectui& rect, const Vector3ui& dstPos = Vector3ui(0U)) const;

			bool Create(VideoMode mode, const String& title, UInt32 style = WindowStyle_Default, const ContextParameters& parameters = ContextParameters());
			bool Create(WindowHandle handle, const ContextParameters& parameters = ContextParameters());

			void Display();

			void EnableVerticalSync(bool enabled);

			unsigned int GetHeight() const override;
			RenderTargetParameters GetParameters() const override;
			unsigned int GetWidth() const override;

			bool IsRenderable() const override;
			bool IsValid() const;

			void SetFramerateLimit(unsigned int limit);

			// Fonctions OpenGL
			ContextParameters GetContextParameters() const;
			bool HasContext() const override;

			RenderWindow& operator=(const RenderWindow&) = delete;
			RenderWindow& operator=(RenderWindow&&) = delete; ///TODO

		protected:
			bool Activate() const override;
			void EnsureTargetUpdated() const override;
			bool OnWindowCreated() override;
			void OnWindowDestroy() override;
			void OnWindowResized() override;

		private:
			mutable std::vector<UInt8> m_buffer;
			Clock m_clock;
			ContextParameters m_parameters;
			mutable Context* m_context = nullptr;
			unsigned int m_framerateLimit = 0;
	};
}

#endif // NAZARA_RENDERWINDOW_HPP
