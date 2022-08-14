// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Web/WebContext.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Web/WebLoader.hpp>
#include <Nazara/Utils/CallOnExit.hpp>
#include <array>
#include <cassert>
#include <cstdlib>
#include <Nazara/OpenGLRenderer/Debug.hpp>

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

		std::size_t configCount;
		std::array<EmscriptenWebGLContextAttributes, 0xFF> configs;
		if (!ChooseConfig(configs.data(), configs.size(), &configCount))
			return false;

		std::size_t configIndex = 0;

		return CreateInternal(configs[configIndex], shareContext);
	}

	bool WebContext::Create(const ContextParams& params, WindowHandle /*window*/, const WebContext* /*shareContext*/)
	{
		/*NazaraError("Unexpected context creation call");
		return false;*/

		return Create(params, nullptr);
	}

	void WebContext::Destroy()
	{
		if (s_handle > 0)
		{
			assert(s_handle > 0);

			OnContextRelease();
			NotifyContextDestruction(this);

			s_handleCounter--;

			if(s_handleCounter == 0)
			{
				emscripten_webgl_destroy_context(s_handle);
				s_handle = 0;
			}
		}
	}

	void WebContext::EnableVerticalSync(bool /*enabled*/)
	{
		// TODO
	}

	void WebContext::SwapBuffers()
	{
		emscripten_webgl_commit_frame();
	}

	bool WebContext::ChooseConfig(EmscriptenWebGLContextAttributes* configs, std::size_t maxConfigCount, std::size_t* configCount)
	{
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

		size_t numConfig = 1;

		configs[0] = configAttributes;

		*configCount = numConfig;

		return true;
	}

	bool WebContext::CreateInternal(EmscriptenWebGLContextAttributes config, const WebContext* shareContext)
	{
		if(s_handleCounter > 0)
		{
			s_handleCounter++;
			return true;
		}

		if (shareContext)
		{
			NazaraWarning(std::string("shared contexes are not supported by WebGL but shareContext is not null"));
		}

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

				s_handle = emscripten_webgl_create_context("canvas", &config);
				if (s_handle > 0)
				{
					break;
				}
			}
		}
		else
		{
			NazaraError(std::string("failed to create WebGL context: OpenGL is not supported"));
			return false;
		}

		if (s_handle <= 0)
		{
			NazaraError(std::string("failed to create Web context: ") + WebLoader::TranslateError(static_cast<EMSCRIPTEN_RESULT>(s_handle)));
			return false;
		}

		LoadExt();

		s_handleCounter++;
		return true;
	}

	bool WebContext::ImplementFallback(const std::string_view& function)
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
		EMSCRIPTEN_RESULT succeeded = emscripten_webgl_make_context_current(s_handle);
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
		if (extensionString)
		{
			SplitString(extensionString, " ", [&](std::string_view extension)
			{
				m_supportedPlatformExtensions.emplace(extension);

				std::string ext(extension);

				emscripten_webgl_enable_extension(s_handle, ext.c_str());

				return true;
			});
		}

		free(extensionString);

		return true;
	}


	EMSCRIPTEN_WEBGL_CONTEXT_HANDLE WebContext::s_handle = 0;
	size_t WebContext::s_handleCounter = 0;
}

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Core/AntiWindows.hpp>
#endif
