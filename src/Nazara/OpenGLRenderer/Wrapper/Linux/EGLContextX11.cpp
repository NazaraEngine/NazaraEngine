// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Linux/EGLContextX11.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLLoader.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	bool EGLContextX11::Create(const ContextParams& params, const EGLContextBase* shareContext)
	{
		Destroy(); //< In case a previous display or surface hasn't been released

		m_params = params;

		if (!BindAPI())
			return false;

		m_xdisplay = XOpenDisplay(nullptr);
		if (!m_xdisplay)
		{
			NazaraError("failed to connect to X server");
			return false;
		}

		m_display = m_loader.eglGetDisplay(m_xdisplay);
		if (!m_display)
		{
			NazaraError("failed to retrieve default EGL display");
			return false;
		}

		if (!InitDisplay())
			return false;

		std::size_t configCount;
		std::array<EGLConfig, 0xFF> configs;
		if (!ChooseConfig(configs.data(), configs.size(), &configCount))
			return false;

		EGLint surfaceAttributes[] = {
			EGL_WIDTH, 1,
			EGL_HEIGHT, 1,
			EGL_NONE
		};

		std::size_t configIndex = 0;
		for (; configIndex < configCount; ++configIndex)
		{
			m_surface = m_loader.eglCreatePbufferSurface(m_display, configs[configIndex], surfaceAttributes);
			if (m_surface)
				break;
		}

		return CreateInternal(configs[configIndex], shareContext);
	}

	bool EGLContextX11::Create(const ContextParams& params, WindowHandle window, const EGLContextBase* shareContext)
	{
		assert(window.type == WindowManager::X11);

		Destroy(); //< In case a previous display or surface hasn't been released

		m_params = params;

		if (!BindAPI())
			return false;

		m_display = m_loader.eglGetDisplay(static_cast<Display*>(window.x11.display));
		if (!InitDisplay())
			return false;

		std::size_t configCount;
		std::array<EGLConfig, 0xFF> configs;
		if (!ChooseConfig(configs.data(), configs.size(), &configCount))
			return false;

		EGLint surfaceAttributes[] = {
			EGL_NONE
		};

		::Window winHandle = static_cast<::Window>(window.x11.window);

		std::size_t configIndex = 0;
		for (; configIndex < configCount; ++configIndex)
		{
			m_surface = m_loader.eglCreateWindowSurface(m_display, configs[configIndex], winHandle, surfaceAttributes);
			if (m_surface)
				break;
		}

		return CreateInternal(configs[configIndex], shareContext);
	}

	void EGLContextX11::Destroy()
	{
		EGLContextBase::Destroy();

		if (m_xdisplay)
		{
			XCloseDisplay(m_xdisplay);
			m_xdisplay = nullptr;
		}
	}
}
