// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Code inspiré de NeHe (Lesson1) et de la SFML par Laurent Gomila

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Win32/ContextImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/LockGuard.hpp>
#include <Nazara/Core/Mutex.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <cstring>
#include <Nazara/Renderer/Debug.hpp>

NzContextImpl::NzContextImpl()
{
}

bool NzContextImpl::Activate()
{
	return wglMakeCurrent(m_deviceContext, m_context);
}

bool NzContextImpl::Create(NzContextParameters& parameters)
{
	if (parameters.window)
	{
		m_window = static_cast<HWND>(parameters.window);
		m_ownsWindow = false;
	}
	else
	{
		m_window = CreateWindowA("STATIC", nullptr, WS_DISABLED | WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
		if (!m_window)
		{
			NazaraError("Failed to create window");
			return false;
		}

		ShowWindow(m_window, SW_HIDE);
		m_ownsWindow = true;
	}

	m_deviceContext = GetDC(m_window);
	if (!m_deviceContext)
	{
		NazaraError("Failed to get device context");
		Destroy();
		return false;
	}

	int pixelFormat = 0;
	if (parameters.antialiasingLevel > 0)
	{
		if (wglChoosePixelFormat)
		{
			bool valid;
			UINT numFormats;

			int attributes[] = {
				WGL_DRAW_TO_WINDOW_ARB,	GL_TRUE,
				WGL_SUPPORT_OPENGL_ARB,	GL_TRUE,
				WGL_ACCELERATION_ARB,	WGL_FULL_ACCELERATION_ARB,
				WGL_COLOR_BITS_ARB,		(parameters.bitsPerPixel == 32) ? 24 : parameters.bitsPerPixel,
				WGL_ALPHA_BITS_ARB,		(parameters.bitsPerPixel == 32) ? 8 : 0,
				WGL_DEPTH_BITS_ARB,		parameters.depthBits,
				WGL_STENCIL_BITS_ARB,	parameters.stencilBits,
				WGL_DOUBLE_BUFFER_ARB,	(parameters.doubleBuffered) ? GL_TRUE : GL_FALSE,
				WGL_SAMPLE_BUFFERS_ARB,	GL_TRUE,
				WGL_SAMPLES_ARB, 		parameters.antialiasingLevel,
				0,						0
			};

			do
			{
				valid = wglChoosePixelFormat(m_deviceContext, attributes, nullptr, 1, &pixelFormat, &numFormats);
			}
			while ((!valid || numFormats == 0) && --attributes[19] > 0);

			if (!valid)
			{
				NazaraWarning("Could not find a format matching requirements, disabling antialiasing...");
				pixelFormat = 0;
			}

			parameters.antialiasingLevel = attributes[19];
		}
		else
		{
			NazaraWarning("Antialiasing is not supported");
			parameters.antialiasingLevel = 0;
		}
	}

	PIXELFORMATDESCRIPTOR descriptor;
	ZeroMemory(&descriptor, sizeof(PIXELFORMATDESCRIPTOR));
	descriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	descriptor.nVersion = 1;

	if (pixelFormat == 0)
	{
		descriptor.cColorBits = parameters.bitsPerPixel;
		descriptor.cDepthBits = parameters.depthBits;
		descriptor.cStencilBits = parameters.stencilBits;
		descriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
		descriptor.iPixelType = PFD_TYPE_RGBA;

		if (parameters.bitsPerPixel == 32)
			descriptor.cAlphaBits = 8;

		if (parameters.doubleBuffered)
			descriptor.dwFlags |= PFD_DOUBLEBUFFER;

		pixelFormat = ChoosePixelFormat(m_deviceContext, &descriptor);
		if (pixelFormat == 0)
		{
			NazaraError("Failed to choose pixel format");
			Destroy();

			return false;
		}
	}

	if (!SetPixelFormat(m_deviceContext, pixelFormat, &descriptor))
	{
		NazaraError("Failed to set pixel format");
		Destroy();
		return false;
	}

	// Arrivé ici, le format de pixel est choisi, nous récupérons donc les paramètres réels du futur contexte
	if (DescribePixelFormat(m_deviceContext, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &descriptor) != 0)
	{
		parameters.bitsPerPixel = descriptor.cColorBits + descriptor.cAlphaBits;
		parameters.depthBits = descriptor.cDepthBits;
		parameters.stencilBits = descriptor.cDepthBits;
	}
	else
		NazaraWarning("Failed to get context's parameters");

	HGLRC shareContext = (parameters.shared) ? static_cast<NzContextImpl*>(parameters.shareContext->m_impl)->m_context : nullptr;

	m_context = nullptr;
	if (wglCreateContextAttribs)
	{
		int attributes[4*2+1];
		int* attrib = attributes;

		*attrib++ = WGL_CONTEXT_MAJOR_VERSION_ARB;
		*attrib++ = parameters.majorVersion;

		*attrib++ = WGL_CONTEXT_MINOR_VERSION_ARB;
		*attrib++ = parameters.minorVersion;

		int flags = 0;

		if (parameters.majorVersion >= 3)
		{
			*attrib++ = WGL_CONTEXT_PROFILE_MASK_ARB;
			if (parameters.compatibilityProfile)
				*attrib++ = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
			else
			{
				*attrib++ = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;

				flags |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
			}
		}

		if (parameters.debugMode)
			flags |= WGL_CONTEXT_DEBUG_BIT_ARB;

		if (flags)
		{
			*attrib++ = WGL_CONTEXT_FLAGS_ARB;
			*attrib++ = flags;
		}

		*attrib++ = 0;

		m_context = wglCreateContextAttribs(m_deviceContext, shareContext, attributes);
	}

	if (!m_context)
	{
		m_context = wglCreateContext(m_deviceContext);

		if (shareContext)
		{
			// wglShareLists n'est pas thread-safe (source: SFML)
			static NzMutex mutex;
			NzLockGuard lock(mutex);

			if (!wglShareLists(shareContext, m_context))
				NazaraWarning("Failed to share the context: " + NzGetLastSystemError());
		}
	}

	if (!m_context)
	{
		NazaraError("Failed to create context");
		Destroy();
		return false;
	}

	return true;
}

void NzContextImpl::Destroy()
{
	if (m_context)
		wglDeleteContext(m_context);

	if (m_deviceContext)
		ReleaseDC(m_window, m_deviceContext);

	if (m_ownsWindow)
		DestroyWindow(m_window);
}

void NzContextImpl::SwapBuffers()
{
	::SwapBuffers(m_deviceContext);
}

bool NzContextImpl::Desactivate()
{
	return wglMakeCurrent(nullptr, nullptr);
}

