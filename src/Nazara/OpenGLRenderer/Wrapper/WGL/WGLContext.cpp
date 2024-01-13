// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/WGL/WGLContext.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/WGL/WGLLoader.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <array>
#include <cassert>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	WGLContext::~WGLContext()
	{
		Destroy();
	}

	bool WGLContext::Create(const WGLContext* baseContext, const ContextParams& params, const WGLContext* shareContext)
	{
		Destroy();

		// Creating a context requires a device context, create window to get one
		HWNDHandle window(::CreateWindowA("STATIC", nullptr, WS_DISABLED | WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, GetModuleHandle(nullptr), nullptr));
		if (!window)
		{
			NazaraErrorFmt("failed to create dummy window: {0}", Error::GetLastSystemError());
			return false;
		}

		::ShowWindow(window.get(), FALSE);

		m_deviceContext = ::GetDC(window.get());
		if (!m_deviceContext)
		{
			NazaraErrorFmt("failed to retrieve dummy window device context: {0}", Error::GetLastSystemError());
			return false;
		}

		if (!CreateInternal(baseContext, params, shareContext))
			return false;

		m_window = std::move(window);
		return true;
	}

	bool WGLContext::Create(const WGLContext* baseContext, const ContextParams& params, WindowHandle window, const WGLContext* shareContext)
	{
		NazaraAssert(window.type == WindowBackend::Windows, "expected Windows window");

		Destroy();

		m_deviceContext = ::GetDC(static_cast<HWND>(window.windows.window));
		if (!m_deviceContext)
		{
			NazaraErrorFmt("failed to retrieve window device context: {0}", Error::GetLastSystemError());
			return false;
		}

		return CreateInternal(baseContext, params, shareContext);
	}

	void WGLContext::Destroy()
	{
		if (m_handle)
		{
			OnContextRelease();
			NotifyContextDestruction(this);

			m_loader.wglDeleteContext(m_handle);
			m_handle = nullptr;
		}
	}

	PresentModeFlags WGLContext::GetSupportedPresentModes() const
	{
		PresentModeFlags supportedModes = PresentMode::Immediate;
		if (wglSwapIntervalEXT)
		{
			supportedModes |= PresentMode::VerticalSync;
			if (HasPlatformExtension("WGL_EXT_swap_control_tear"))
				supportedModes |= PresentMode::RelaxedVerticalSync;
		}

		return supportedModes;
	}

	void WGLContext::SetPresentMode(PresentMode presentMode)
	{
		if (!SetCurrentContext(this))
			return;

		int interval = 0;
		switch (presentMode)
		{
			case PresentMode::Immediate:           interval = 0;  break;
			case PresentMode::RelaxedVerticalSync: interval = -1; break; //< WGL_EXT_swap_control_tear
			case PresentMode::VerticalSync:        interval = 1;  break;
			default: return; // TODO: Unreachable
		}

		wglSwapIntervalEXT(interval);
	}

	void WGLContext::SwapBuffers()
	{
		m_loader.SwapBuffers(m_deviceContext);
	}

	bool WGLContext::CreateInternal(const WGLContext* baseContext, const ContextParams& params, const WGLContext* shareContext)
	{
		Destroy();

		m_params = params;

		if (!SetPixelFormat())
			return false;

		if (baseContext && baseContext->wglCreateContextAttribsARB)
		{
			struct Version
			{
				unsigned int major;
				unsigned int minor;
			};

			if (params.type == ContextType::OpenGL_ES)
			{
				if (baseContext->HasPlatformExtension("WGL_EXT_create_context_es_profile"))
				{
					// Create OpenGL ES context
					std::array<Version, 3> supportedGL_ESVersions = {
						{
							{ 3, 2 },
							{ 3, 1 },
							{ 3, 0 }
						}
					};

					for (const Version& version : supportedGL_ESVersions)
					{
						if (params.glMajorVersion != 0)
						{
							if (version.major > params.glMajorVersion)
								continue;

							if (params.glMinorVersion != 0 && version.minor > params.glMinorVersion)
								continue;
						}

						std::array<int, 3 * 2 + 1> attributes = {
							WGL_CONTEXT_MAJOR_VERSION_ARB, int(version.major),
							WGL_CONTEXT_MINOR_VERSION_ARB, int(version.minor),

							WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_ES_PROFILE_BIT_EXT
						};

						m_handle = baseContext->wglCreateContextAttribsARB(m_deviceContext, (shareContext) ? shareContext->m_handle : nullptr, attributes.data());
						if (m_handle)
							break;
					}
				}
			}

			if (!m_handle)
			{
				// Create OpenGL context
				std::array<Version, 8> supportedGLVersions = {
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

				for (const Version& version : supportedGLVersions)
				{
					if (params.glMajorVersion != 0)
					{
						if (version.major > params.glMajorVersion)
							continue;

						if (params.glMinorVersion != 0 && version.minor > params.glMinorVersion)
							continue;
					}

					std::array<int, 3 * 2 + 1> attributes = {
						WGL_CONTEXT_MAJOR_VERSION_ARB, int(version.major),
						WGL_CONTEXT_MINOR_VERSION_ARB, int(version.minor),

						WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB
					};

					m_handle = baseContext->wglCreateContextAttribsARB(m_deviceContext, (shareContext) ? shareContext->m_handle : nullptr, attributes.data());
					if (m_handle)
					{
						m_params.type = ContextType::OpenGL;
						break;
					}
				}
			}

			if (!m_handle)
			{
				NazaraErrorFmt("failed to create WGL context: {0}", Error::GetLastSystemError());
				return false;
			}
		}
		else
		{
			m_handle = m_loader.wglCreateContext(m_deviceContext);
			if (!m_handle)
			{
				NazaraErrorFmt("failed to create WGL context: {0}", Error::GetLastSystemError());
				return false;
			}

			if (shareContext)
			{
				if (!m_loader.wglShareLists(shareContext->m_handle, m_handle))
				{
					NazaraErrorFmt("failed to share context objects: {0}", Error::GetLastSystemError());
					return false;
				}
			}

			m_params.type = ContextType::OpenGL;
		}

		LoadWGLExt();

		return true;
	}

	bool WGLContext::ImplementFallback(std::string_view function)
	{
		if (Context::ImplementFallback(function))
			return true;

		if (m_params.type == ContextType::OpenGL_ES)
			return false; //< Implement fallback only for OpenGL (when emulating OpenGL ES)

		if (function == "glClearDepthf")
		{
			fallbacks.glClearDepth = reinterpret_cast<Fallback::glClearDepthProc>(m_loader.LoadFunction("glClearDepth"));
			if (!fallbacks.glClearDepth)
				return false;

			glClearDepthf = [](GLfloat depth)
			{
				const WGLContext* context = SafeCast<const WGLContext*>(GetCurrentContext());
				assert(context);
				context->fallbacks.glClearDepth(depth);
			};
		}

		return true;
	}

	bool WGLContext::Activate() const
	{
		bool succeeded = m_loader.wglMakeCurrent(m_deviceContext, m_handle);
		if (!succeeded)
		{
			NazaraErrorFmt("failed to activate context: {0}", Error::GetLastSystemError());
			return false;
		}

		return true;
	}

	void WGLContext::Desactivate() const
	{
		assert(GetCurrentContext() == this);
		m_loader.wglMakeCurrent(nullptr, nullptr);
	}

	const Loader& WGLContext::GetLoader()
	{
		return m_loader;
	}

	bool WGLContext::LoadWGLExt()
	{
		if (!SetCurrentContext(this))
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
				m_supportedPlatformExtensions.emplace(extension);
				return true;
			});
		}

		return true;
	}

	bool WGLContext::SetPixelFormat()
	{
		PIXELFORMATDESCRIPTOR descriptor = {};
		descriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		descriptor.nVersion = 1;

		int pixelFormat = 0;
		if (m_params.sampleCount > 1)
		{
			const WGLContext* currentContext = SafeCast<const WGLContext*>(GetCurrentContext()); //< Pay TLS cost only once
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
						WGL_COLOR_BITS_ARB,     int((m_params.bitsPerPixel == 32) ? 24 : m_params.bitsPerPixel),
						WGL_ALPHA_BITS_ARB,     int((m_params.bitsPerPixel == 32) ? 8 : 0),
						WGL_DEPTH_BITS_ARB,     int(m_params.depthBits),
						WGL_STENCIL_BITS_ARB,   int(m_params.stencilBits),
						WGL_DOUBLE_BUFFER_ARB,  int((m_params.doubleBuffering) ? GL_TRUE : GL_FALSE),
						WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
						WGL_SAMPLES_ARB,        int(m_params.sampleCount)
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

					if (int(m_params.sampleCount) != sampleCount)
						NazaraWarning("couldn't find a pixel format matching " + std::to_string(m_params.sampleCount) + " sample count, using " + std::to_string(sampleCount) + " sample(s) instead");

					m_params.sampleCount = sampleCount;
				}
			}
		}

		if (pixelFormat == 0)
		{
			descriptor.cColorBits = BYTE((m_params.bitsPerPixel == 32) ? 24 : m_params.bitsPerPixel);
			descriptor.cDepthBits = BYTE(m_params.depthBits);
			descriptor.cStencilBits = BYTE(m_params.stencilBits);
			descriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
			descriptor.iPixelType = PFD_TYPE_RGBA;

			if (m_params.bitsPerPixel == 32)
				descriptor.cAlphaBits = 8;

			if (m_params.doubleBuffering)
				descriptor.dwFlags |= PFD_DOUBLEBUFFER;

			pixelFormat = m_loader.ChoosePixelFormat(m_deviceContext, &descriptor);
			if (pixelFormat == 0)
			{
				NazaraErrorFmt("failed to choose pixel format: {0}", Error::GetLastSystemError());
				return false;
			}
		}

		if (!m_loader.SetPixelFormat(m_deviceContext, pixelFormat, &descriptor))
		{
			NazaraErrorFmt("failed to choose pixel format: {0}", Error::GetLastSystemError());
			return false;
		}

		if (m_loader.DescribePixelFormat(m_deviceContext, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &descriptor) != 0)
		{
			m_params.bitsPerPixel = descriptor.cColorBits + descriptor.cAlphaBits;
			m_params.depthBits = descriptor.cDepthBits;
			m_params.stencilBits = descriptor.cStencilBits;
		}

		return true;
	}
}

#include <Nazara/Core/AntiWindows.hpp>
