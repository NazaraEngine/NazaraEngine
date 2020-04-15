// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Win32/WGLContext.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Win32/WGLLoader.hpp>
#include <array>
#include <cassert>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	thread_local WGLContext* s_currentContext = nullptr;

	GL::WGLContext::WGLContext(WGLLoader& loader) :
	m_loader(loader)
	{
	}

	WGLContext::~WGLContext()
	{
		Destroy();
	}

	bool WGLContext::Activate()
	{
		WGLContext*& currentContext = s_currentContext; //< Pay TLS cost only once
		if (currentContext)
		{
			if (currentContext == this)
				return true;

			// Only one context can be activated per thread
			currentContext->Desactivate();
		}

		bool succeeded = m_loader.wglMakeCurrent(m_deviceContext, m_handle);
		if (!succeeded)
		{
			NazaraError("failed to activate context: " + Error::GetLastSystemError());
			return false;
		}

		currentContext = this;

		return true;
	}

	bool WGLContext::Create(const ContextParams& params)
	{
		Destroy();

		// Creating a context requires a Window
		m_window.reset(::CreateWindowA("STATIC", nullptr, WS_DISABLED | WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, GetModuleHandle(nullptr), nullptr));
		if (!m_window)
		{
			NazaraError("failed to create dummy window: " + Error::GetLastSystemError());
			return false;
		}

		::ShowWindow(m_window.get(), FALSE);

		m_deviceContext = ::GetDC(m_window.get());
		if (!m_deviceContext)
		{
			NazaraError("failed to retrieve dummy window device context: " + Error::GetLastSystemError());
			return false;
		}

		if (!SetPixelFormat(params))
			return false;

		WGLContext* currentContext = s_currentContext; //< Pay TLS cost only once
		if (currentContext && currentContext->wglCreateContextAttribsARB)
		{
			struct OpenGLVersion
			{
				int major;
				int minor;
			};

			std::array<OpenGLVersion, 8> supportedVersions = {
				{
					{ 4, 6 },
					{ 4, 5 },
					{ 4, 4 },
					{ 4, 3 },
					{ 4, 2 },
					{ 4, 1 },
					{ 4, 0 },
					{ 3, 3 }
				}
			};

			for (const OpenGLVersion& version : supportedVersions)
			{
				std::array<int, 3 * 2 + 1> attributes = {
					WGL_CONTEXT_MAJOR_VERSION_ARB, version.major,
					WGL_CONTEXT_MINOR_VERSION_ARB, version.minor,

					WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB
				};

				m_handle = currentContext->wglCreateContextAttribsARB(m_deviceContext, nullptr, attributes.data());
				if (m_handle)
					break;
			}

			if (!m_handle)
			{
				NazaraError("failed to create WGL context: " + Error::GetLastSystemError());
				return false;
			}
		}
		else
		{
			m_handle = m_loader.wglCreateContext(m_deviceContext);
			if (!m_handle)
			{
				NazaraError("failed to create WGL context: " + Error::GetLastSystemError());
				return false;
			}
		}

		LoadWGLExt();

		return true;
	}

	void WGLContext::Destroy()
	{
		if (m_handle)
		{
			WGLContext*& currentContext = s_currentContext; //< Pay TLS cost only once
			if (currentContext == this)
				currentContext = nullptr;

			m_loader.wglDeleteContext(m_handle);
			m_handle = nullptr;
		}
	}

	void WGLContext::EnableVerticalSync(bool enabled)
	{
	}

	void WGLContext::SwapBuffers()
	{
		m_loader.SwapBuffers(m_deviceContext);
	}

	void WGLContext::Desactivate()
	{
		WGLContext*& currentContext = s_currentContext; //< Pay TLS cost only once
		if (currentContext == this)
		{
			m_loader.wglMakeCurrent(nullptr, nullptr);
			currentContext = nullptr;
		}
	}

	bool WGLContext::LoadWGLExt()
	{
		if (!Activate())
			return false;

#define NAZARA_OPENGLRENDERER_FUNC(name, sig)
#define NAZARA_OPENGLRENDERER_EXT_BEGIN(ext)
#define NAZARA_OPENGLRENDERER_EXT_END()
#define NAZARA_OPENGLRENDERER_EXT_FUNC(name, sig) name = reinterpret_cast<sig>(m_loader.wglGetProcAddress(#name));
		NAZARA_OPENGLRENDERER_FOREACH_WGL_FUNC(NAZARA_OPENGLRENDERER_FUNC, NAZARA_OPENGLRENDERER_EXT_BEGIN, NAZARA_OPENGLRENDERER_EXT_END, NAZARA_OPENGLRENDERER_EXT_FUNC)
#undef NAZARA_OPENGLRENDERER_EXT_BEGIN
#undef NAZARA_OPENGLRENDERER_EXT_END
#undef NAZARA_OPENGLRENDERER_EXT_FUNC
#undef NAZARA_OPENGLRENDERER_FUNC

		const char* extensionString = nullptr;
		if (wglGetExtensionsStringARB)
			extensionString = wglGetExtensionsStringARB(m_deviceContext);
		else if (wglGetExtensionsStringEXT)
			extensionString = wglGetExtensionsStringEXT();

		if (extensionString)
		{
			SplitString(extensionString, " ", [&](std::string_view extension)
			{
				m_supportedExtensions.emplace(extension);
				return true;
			});
		}

		return true;
	}

	bool WGLContext::SetPixelFormat(const ContextParams& params)
	{
		PIXELFORMATDESCRIPTOR descriptor = {};
		descriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		descriptor.nVersion = 1;

		int pixelFormat = 0;
		if (params.sampleCount > 1)
		{
			WGLContext* currentContext = s_currentContext; //< Pay TLS cost only once
			if (currentContext)
			{
				// WGL_ARB_pixel_format and WGL_EXT_pixel_format are the same, except for the symbol
				auto wglChoosePixelFormat = (currentContext->wglChoosePixelFormatARB) ? currentContext->wglChoosePixelFormatARB : currentContext->wglChoosePixelFormatEXT;
				
				if (wglChoosePixelFormat)
				{
					std::array<int, 10 * 2 + 1> attributes = {
						WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
						WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
						WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
						WGL_COLOR_BITS_ARB,     (params.bitsPerPixel == 32) ? 24 : params.bitsPerPixel,
						WGL_ALPHA_BITS_ARB,     (params.bitsPerPixel == 32) ? 8 : 0,
						WGL_DEPTH_BITS_ARB,     params.depthBits,
						WGL_STENCIL_BITS_ARB,   params.stencilBits,
						WGL_DOUBLE_BUFFER_ARB,  (params.doubleBuffering) ? GL_TRUE : GL_FALSE,
						WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
						WGL_SAMPLES_ARB,        params.sampleCount
					};

					int& sampleCount = attributes[attributes.size() - 3];

					do
					{
						UINT formatCount;
						if (currentContext->wglChoosePixelFormatARB(m_deviceContext, attributes.data(), nullptr, 1, &pixelFormat, &formatCount))
						{
							if (formatCount > 0)
								break;
						}

						sampleCount--;
					}
					while (sampleCount > 1);

					if (params.sampleCount != sampleCount)
						NazaraWarning("couldn't find a pixel format matching " + std::to_string(params.sampleCount) + " sample count, using " + std::to_string(sampleCount) + " sample(s) instead");
				}
			}
		}

		if (pixelFormat == 0)
		{
			descriptor.cColorBits = BYTE((params.bitsPerPixel == 32) ? 24 : params.bitsPerPixel);
			descriptor.cDepthBits = BYTE(params.depthBits);
			descriptor.cStencilBits = BYTE(params.stencilBits);
			descriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
			descriptor.iPixelType = PFD_TYPE_RGBA;

			if (params.bitsPerPixel == 32)
				descriptor.cAlphaBits = 8;

			if (params.doubleBuffering)
				descriptor.dwFlags |= PFD_DOUBLEBUFFER;

			pixelFormat = m_loader.ChoosePixelFormat(m_deviceContext, &descriptor);
			if (pixelFormat == 0)
			{
				NazaraError("Failed to choose pixel format: " + Error::GetLastSystemError());
				return false;
			}
		}

		if (!m_loader.SetPixelFormat(m_deviceContext, pixelFormat, &descriptor))
		{
			NazaraError("Failed to choose pixel format: " + Error::GetLastSystemError());
			return false;
		}

		return true;
	}
}
