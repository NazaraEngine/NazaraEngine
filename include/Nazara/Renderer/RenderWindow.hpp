// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

// Interface inspirée de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_RENDERWINDOW_HPP
#define NAZARA_RENDERWINDOW_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Utility/Window.hpp>

#ifndef NAZARA_RENDERER_COMMON
#include <Nazara/Renderer/ContextParameters.hpp>
#endif

class NzContext;
struct NzContextParameters;

class NAZARA_API NzRenderWindow : public NzRenderTarget, public NzWindow
{
	public:
		NzRenderWindow();
		NzRenderWindow(NzVideoMode mode, const NzString& title, nzUInt32 style = NzWindow::Default, const NzContextParameters& parameters = NzContextParameters());
		NzRenderWindow(NzWindowHandle handle, const NzContextParameters& parameters = NzContextParameters());
		virtual ~NzRenderWindow();

		bool CanActivate() const;

		bool Create(NzVideoMode mode, const NzString& title, nzUInt32 style = NzWindow::Default, const NzContextParameters& parameters = NzContextParameters());
		bool Create(NzWindowHandle handle, const NzContextParameters& parameters = NzContextParameters());

		void Display();

		void EnableVerticalSync(bool enabled);

		NzRenderTargetParameters GetRenderTargetParameters() const;

		#ifndef NAZARA_RENDERER_COMMON
		NzContextParameters GetContextParameters() const;

		bool HasContext() const;
		#endif

	protected:
		bool Activate();

	private:
		void OnClose();
		bool OnCreate();

		NzContext* m_context;
		NzContextParameters m_parameters;
};

#endif // NAZARA_RENDERWINDOW_HPP
