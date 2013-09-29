// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Interface inspirée de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_RENDERWINDOW_HPP
#define NAZARA_RENDERWINDOW_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/ContextParameters.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Utility/Window.hpp>

class NzContext;
class NzImage;
class NzTexture;
struct NzContextParameters;

class NAZARA_API NzRenderWindow : public NzRenderTarget, public NzWindow
{
	public:
		NzRenderWindow() = default;
		NzRenderWindow(NzVideoMode mode, const NzString& title, nzUInt32 style = nzWindowStyle_Default, const NzContextParameters& parameters = NzContextParameters());
		NzRenderWindow(NzWindowHandle handle, const NzContextParameters& parameters = NzContextParameters());
		virtual ~NzRenderWindow();

		bool CopyToImage(NzImage* image) const;
		bool CopyToTexture(NzTexture* texture) const;

		bool Create(NzVideoMode mode, const NzString& title, nzUInt32 style = nzWindowStyle_Default, const NzContextParameters& parameters = NzContextParameters());
		bool Create(NzWindowHandle handle, const NzContextParameters& parameters = NzContextParameters());

		void Display();

		void EnableVerticalSync(bool enabled);

		unsigned int GetHeight() const;
		NzRenderTargetParameters GetParameters() const;
		unsigned int GetWidth() const;

		bool IsRenderable() const;
		bool IsValid() const;

		void SetFramerateLimit(unsigned int limit);

		// Fonctions OpenGL
		NzContextParameters GetContextParameters() const;
		bool HasContext() const;

	protected:
		bool Activate() const override;
		void EnsureTargetUpdated() const override;

	private:
		bool OnWindowCreated() override;
		void OnWindowDestroy() override;
		void OnWindowResized() override;

		NzClock m_clock;
		NzContextParameters m_parameters;
		mutable NzContext* m_context = nullptr;
		unsigned int m_framerateLimit = 0;
};

#endif // NAZARA_RENDERWINDOW_HPP
