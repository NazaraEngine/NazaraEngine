// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLContextBase.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLLoader.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <array>
#include <cassert>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	EGLContextBase::~EGLContextBase()
	{
		Destroy();
	}

	bool EGLContextBase::Create(const ContextParams& params, const EGLContextBase* shareContext)
	{
		Destroy(); //< In case a previous display or surface hasn't been released

		m_params = params;

		if (!BindAPI())
			return false;

		m_display = m_loader.GetDefaultDisplay();

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

	bool EGLContextBase::Create(const ContextParams& /*params*/, WindowHandle /*window*/, const EGLContextBase* /*shareContext*/)
	{
		NazaraError("unexpected context creation call");
		return false;
	}

	void EGLContextBase::Destroy()
	{
		if (m_handle != EGL_NO_CONTEXT)
		{
			assert(m_display != EGL_NO_DISPLAY);

			OnContextRelease();
			NotifyContextDestruction(this);

			m_loader.eglDestroyContext(m_display, m_handle);
			m_handle = EGL_NO_CONTEXT;
		}

		if (m_surface != EGL_NO_SURFACE)
		{
			assert(m_display != EGL_NO_DISPLAY);
			m_loader.eglDestroySurface(m_display, m_surface);
			m_surface = EGL_NO_SURFACE;
		}

		if (m_display)
		{
			if (m_ownsDisplay)
				m_loader.eglTerminate(m_display);

			m_display = EGL_NO_DISPLAY;
			m_ownsDisplay = false;
		}
	}

	PresentModeFlags EGLContextBase::GetSupportedPresentModes() const
	{
		PresentModeFlags supportedModes;
		if (m_maxSwapInterval >= 1)
			supportedModes |= PresentMode::VerticalSync;

		if (m_minSwapInterval <= 0)
			supportedModes |= PresentMode::Immediate;

		if (m_minSwapInterval <= -1)
			supportedModes |= PresentMode::RelaxedVerticalSync;

		return supportedModes;
	}

	void EGLContextBase::SetPresentMode(PresentMode presentMode)
	{
		int interval = 0;
		switch (presentMode)
		{
			case PresentMode::Immediate:           interval = 0;  break;
			case PresentMode::RelaxedVerticalSync: interval = -1; break;
			case PresentMode::VerticalSync:        interval = 1;  break;
			default: return; // TODO: Unreachable
		}

		m_loader.eglSwapInterval(m_display, interval);
	}

	void EGLContextBase::SwapBuffers()
	{
		m_loader.eglSwapBuffers(m_display, m_surface);
	}

	bool EGLContextBase::BindAPI()
	{
		if (m_params.type == ContextType::OpenGL_ES)
		{
			if (m_loader.eglBindAPI(EGL_OPENGL_ES_API) == EGL_TRUE)
				return true;

			if (m_loader.eglBindAPI(EGL_OPENGL_API) == EGL_TRUE)
			{
				m_params.type = ContextType::OpenGL;
				return true;
			}

			NazaraError("neither OpenGL nor OpenGL ES are supported");
			return false;
		}
		else
		{
			if (m_loader.eglBindAPI(EGL_OPENGL_API) != EGL_TRUE)
			{
				NazaraError("OpenGL is not supported");
				return false;
			}

			return true;
		}
	}

	bool EGLContextBase::ChooseConfig(EGLConfig* configs, std::size_t maxConfigCount, std::size_t* configCount)
	{
		EGLint configAttributes[] =
		{
			EGL_BUFFER_SIZE, EGLint(m_params.bitsPerPixel),
			EGL_DEPTH_SIZE, EGLint(m_params.depthBits),
			EGL_RENDERABLE_TYPE, (m_params.type == ContextType::OpenGL_ES) ? EGL_OPENGL_ES3_BIT : EGL_OPENGL_BIT,
			EGL_SAMPLE_BUFFERS, EGLint((m_params.sampleCount > 1) ? 1 : 0),
			EGL_SAMPLES, EGLint((m_params.sampleCount > 1) ? m_params.sampleCount : 0),
			EGL_SURFACE_TYPE, EGL_PBUFFER_BIT | EGL_WINDOW_BIT,
			EGL_STENCIL_SIZE, EGLint(m_params.stencilBits),
			EGL_NONE
		};

		EGLint numConfig = 0;
		if (m_loader.eglChooseConfig(m_display, configAttributes, configs, EGLint(maxConfigCount), &numConfig) != GL_TRUE)
		{
			NazaraErrorFmt("failed to retrieve compatible EGL configurations: {0}", EGLLoader::TranslateError(m_loader.eglGetError()));
			return false;
		}

		if (numConfig == 0)
		{
			NazaraError("no supported configuration matches required attributes");
			return false;
		}

		*configCount = numConfig;

		return true;
	}

	bool EGLContextBase::CreateInternal(EGLConfig config, const EGLContextBase* shareContext)
	{
		struct Version
		{
			unsigned int major;
			unsigned int minor;
		};

		if (m_params.type == ContextType::OpenGL_ES)
		{
			// Create OpenGL ES context
			constexpr std::array<Version, 3> supportedGL_ESVersions = {
				{
					{ 3, 2 },
					{ 3, 1 },
					{ 3, 0 }
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

				std::array<int, 2 * 2 + 1> attributes = {
					EGL_CONTEXT_MAJOR_VERSION, int(version.major),
					EGL_CONTEXT_MINOR_VERSION, int(version.minor),

					EGL_NONE
				};

				m_handle = m_loader.eglCreateContext(m_display, config, (shareContext) ? shareContext->m_handle : nullptr, attributes.data());
				if (m_handle)
					break;
			}
		}
		else
		{
			// Create OpenGL ES context
			constexpr std::array<Version, 8> supportedGLVersions = {
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
				if (m_params.glMajorVersion != 0)
				{
					if (version.major > m_params.glMajorVersion)
						continue;

					if (m_params.glMinorVersion != 0 && version.minor > m_params.glMinorVersion)
						continue;
				}

				std::array<int, 3 * 2 + 1> attributes = {
					EGL_CONTEXT_MAJOR_VERSION, int(version.major),
					EGL_CONTEXT_MINOR_VERSION, int(version.minor),

					EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,

					EGL_NONE
				};

				m_handle = m_loader.eglCreateContext(m_display, config, (shareContext) ? shareContext->m_handle : nullptr, attributes.data());
				if (m_handle)
					break;
			}
		}

		if (!m_handle)
		{
			NazaraErrorFmt("failed to create EGL context: {0}", EGLLoader::TranslateError(m_loader.eglGetError()));
			return false;
		}

		m_loader.eglGetConfigAttrib(m_display, config, EGL_MAX_SWAP_INTERVAL, &m_maxSwapInterval);
		m_loader.eglGetConfigAttrib(m_display, config, EGL_MIN_SWAP_INTERVAL, &m_minSwapInterval);

		LoadEGLExt();

		return true;
	}

	bool EGLContextBase::InitDisplay()
	{
		EGLint major, minor;
		if (m_loader.eglInitialize(m_display, &major, &minor) != EGL_TRUE)
		{
			NazaraError("failed to retrieve default EGL display");
			return false;
		}

		m_ownsDisplay = true;

		const char* vendor = m_loader.eglQueryString(m_display, EGL_VENDOR);
		NazaraNotice("Initialized EGL " + std::to_string(major) + "." + std::to_string(minor) + " display (" + vendor + ")");

		return true;
	}

	bool EGLContextBase::ImplementFallback(std::string_view function)
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
				const EGLContextBase* context = SafeCast<const EGLContextBase*>(GetCurrentContext());
				assert(context);
				context->fallbacks.glClearDepth(depth);
			};
		}

		return true;
	}

	bool EGLContextBase::Activate() const
	{
		EGLBoolean succeeded = m_loader.eglMakeCurrent(m_display, m_surface, m_surface, m_handle);
		if (succeeded != EGL_TRUE)
		{
			NazaraError("failed to activate context");
			return false;
		}

		return true;
	}

	void EGLContextBase::Desactivate() const
	{
		assert(GetCurrentContext() == this);
		EGLBoolean succeeded = m_loader.eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (succeeded != EGL_TRUE)
			NazaraError("failed to desactivate context");
	}

	const Loader& EGLContextBase::GetLoader()
	{
		return m_loader;
	}

	bool EGLContextBase::LoadEGLExt()
	{
		if (!SetCurrentContext(this))
			return false;

		const char* extensionString = m_loader.eglQueryString(m_display, EGL_EXTENSIONS);
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
}

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Core/AntiWindows.hpp>
#endif
