// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Web/WebContext.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Web/WebLoader.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <array>
#include <cassert>
#include <cstdlib>

namespace Nz::GL
{
	WebContext::~WebContext()
	{
		Destroy();
	}

	bool WebContext::Create(const ContextParams& params, const WebContext* shareContext)
	{
		Destroy(); //< In case a previous display or surface hasn't been released

		m_params = params;

		EmscriptenWebGLContextAttributes configAttributes;
		emscripten_webgl_init_context_attributes(&configAttributes);

		// https://emscripten.org/docs/api_reference/html5.h.html#c.EmscriptenWebGLContextAttributes
		configAttributes.alpha = true;
		configAttributes.depth = m_params.depthBits > 0;
		configAttributes.stencil = m_params.stencilBits > 0;
		configAttributes.antialias = false;
		configAttributes.premultipliedAlpha = true;
		configAttributes.preserveDrawingBuffer = false;
		configAttributes.powerPreference = EM_WEBGL_POWER_PREFERENCE_DEFAULT;
		configAttributes.failIfMajorPerformanceCaveat = false;
		configAttributes.majorVersion = m_params.glMajorVersion;
		configAttributes.minorVersion = m_params.glMinorVersion;
		configAttributes.enableExtensionsByDefault = true;
		configAttributes.explicitSwapControl = false; // todo
		configAttributes.renderViaOffscreenBackBuffer = false; // todo
		configAttributes.proxyContextToMainThread = false; // todo

		return CreateInternal(configAttributes, shareContext);
	}

	bool WebContext::Create(const ContextParams& params, WindowHandle /*window*/, const WebContext* /*shareContext*/)
	{
		return Create(params, nullptr);
	}

	void WebContext::Destroy()
	{
		if (m_handle != 0)
		{
			OnContextRelease();
			NotifyContextDestruction(this);

			emscripten_webgl_destroy_context(m_handle);
			m_handle = 0;
		}
	}

	PresentModeFlags WebContext::GetSupportedPresentModes() const
	{
		// WebGL does not support disabling V-Sync
		return PresentMode::VerticalSync;
	}

	void WebContext::SetPresentMode(PresentMode /*presentMode*/)
	{
		// Nothing to do
	}

	void WebContext::SwapBuffers()
	{
		emscripten_webgl_commit_frame();
	}

	bool WebContext::CreateInternal(EmscriptenWebGLContextAttributes config, const WebContext* shareContext)
	{
		NazaraCheck(shareContext == nullptr, "shared contexes are not supported by WebGL but shareContext is not null");

		struct Version
		{
			unsigned int major;
			unsigned int minor;
		};

		if (m_params.type == ContextType::OpenGL_ES)
		{
			// Create OpenGL ES context
			std::array<Version, 3> supportedGL_ESVersions = {
				{
					{ 2, 0 },
					{ 1, 0 }
				}
			};

			for (const Version& version : supportedGL_ESVersions)
			{
				if (m_params.glMajorVersion != 0)
				{
					if (version.major > m_params.glMajorVersion)
						continue;

					if (m_params.glMinorVersion != 0 && version.minor > m_params.glMinorVersion)
						continue;
				}

				config.majorVersion = version.major;
				config.minorVersion = version.minor;

				m_handle = emscripten_webgl_create_context("canvas", &config);
				if (m_handle > 0)
				{
					break;
				}
			}
		}
		else
		{
			NazaraError("failed to create WebGL context: OpenGL is not supported");
			return false;
		}

		if (m_handle <= 0)
		{
			NazaraError("failed to create Web context: {0}", WebLoader::TranslateError(static_cast<EMSCRIPTEN_RESULT>(m_handle)));
			return false;
		}

		LoadExt();
		return true;
	}

	bool WebContext::ImplementFallback(std::string_view function)
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
				const WebContext* context = static_cast<const WebContext*>(GetCurrentContext());
				assert(context);
				context->fallbacks.glClearDepth(depth);
			};
		}

		return true;
	}

	bool WebContext::Activate() const
	{
		EMSCRIPTEN_RESULT succeeded = emscripten_webgl_make_context_current(m_handle);
		if (succeeded != EMSCRIPTEN_RESULT_SUCCESS)
		{
			NazaraError("failed to activate context");
			return false;
		}

		return true;
	}

	void WebContext::Desactivate() const
	{
		assert(GetCurrentContext() == this);
		EMSCRIPTEN_RESULT succeeded = emscripten_webgl_make_context_current(0);
		if (succeeded != EMSCRIPTEN_RESULT_SUCCESS)
			NazaraError("failed to desactivate context");
	}

	const Loader& WebContext::GetLoader()
	{
		return m_loader;
	}

	bool WebContext::LoadExt()
	{
		if (!SetCurrentContext(this))
			return false;

		char* extensionString = emscripten_webgl_get_supported_extensions();
		CallOnExit releaseStr([&] { free(extensionString); });

		if (extensionString)
		{
			SplitString(extensionString, " ", [&](std::string_view extension)
			{
				m_supportedPlatformExtensions.emplace(extension);

				std::string ext(extension);

				emscripten_webgl_enable_extension(m_handle, ext.c_str());

				return true;
			});
		}

		return true;
	}
}

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Core/AntiWindows.hpp>
#endif
