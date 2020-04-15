// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#if 0

#include <Nazara/OpenGLRenderer/OpenGLSurface.hpp>
#include <Nazara/OpenGLRenderer/OpenGL.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLSurface::OpenGLSurface() :
	m_surface(OpenGL::GetInstance())
	{
	}

	OpenGLSurface::~OpenGLSurface() = default;

	bool OpenGLSurface::Create(WindowHandle handle)
	{
		bool success = false;
		#if defined(NAZARA_PLATFORM_WINDOWS)
		{
			HWND winHandle = reinterpret_cast<HWND>(handle);
			HINSTANCE instance = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(winHandle, GWLP_HINSTANCE));

			success = m_surface.Create(instance, winHandle);
		}
		#else
		#error This OS is not supported by OpenGL
		#endif

		if (!success)
		{
			NazaraError("Failed to create OpenGL surface: " + TranslateOpenGLError(m_surface.GetLastErrorCode()));
			return false;
		}

		return true;
	}

	void OpenGLSurface::Destroy()
	{
		m_surface.Destroy();
	}
}

#endif
