// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLLoader.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLContextBase.hpp>

#ifdef NAZARA_PLATFORM_LINUX
#include <Nazara/OpenGLRenderer/Wrapper/Linux/EGLContextX11.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Linux/EGLContextWayland.hpp>
#endif

#ifdef NAZARA_PLATFORM_WINDOWS
#include <Nazara/OpenGLRenderer/Wrapper/Win32/EGLContextWin32.hpp>
#endif

#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	EGLLoader::EGLLoader() :
	m_defaultDisplay(nullptr)
	{
		if (!m_eglLib.Load("libEGL"))
			throw std::runtime_error("failed to load gdi32.dll: " + m_eglLib.GetLastError());

		auto LoadSymbol = [](DynLib& lib, auto& func, const char* funcName)
		{
			func = reinterpret_cast<std::decay_t<decltype(func)>>(lib.GetSymbol(funcName));
			if (!func)
				throw std::runtime_error("failed to load core function " + std::string(funcName));
		};

		// Load gdi32 functions
#define NAZARA_OPENGLRENDERER_EXT_BEGIN(ext)
#define NAZARA_OPENGLRENDERER_EXT_END()
#define NAZARA_OPENGLRENDERER_EXT_FUNC(name, sig) //< Ignore extensions

		// Load base WGL functions
#define NAZARA_OPENGLRENDERER_FUNC(name, sig) LoadSymbol(m_eglLib, name, #name);
		NAZARA_OPENGLRENDERER_FOREACH_EGL_FUNC(NAZARA_OPENGLRENDERER_FUNC, NAZARA_OPENGLRENDERER_EXT_BEGIN, NAZARA_OPENGLRENDERER_EXT_END, NAZARA_OPENGLRENDERER_EXT_FUNC)
#undef NAZARA_OPENGLRENDERER_FUNC

#undef NAZARA_OPENGLRENDERER_EXT_BEGIN
#undef NAZARA_OPENGLRENDERER_EXT_END
#undef NAZARA_OPENGLRENDERER_EXT_FUNC

		EGLDisplay defaultDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		if (!defaultDisplay)
			throw std::runtime_error("failed to retrieve default EGL display");

		EGLint major, minor;
		if (eglInitialize(defaultDisplay, &major, &minor) != EGL_TRUE)
			throw std::runtime_error("failed to initialize default EGL display");

		m_defaultDisplay = defaultDisplay;

		const char* vendor = eglQueryString(m_defaultDisplay, EGL_VENDOR);
		NazaraNotice("Initialized EGL " + std::to_string(major) + "." + std::to_string(minor) + " display (" + vendor + ")");

		// Try to create a dummy context in order to check EGL support (FIXME: is this really necessary?)
		/*ContextParams params;
		EGLContextBase baseContext(nullptr, *this);
		if (!baseContext.Create(params))
			throw std::runtime_error("failed to create load context");

		if (!baseContext.Initialize(params))
			throw std::runtime_error("failed to load OpenGL functions");*/
	}

	EGLLoader::~EGLLoader()
	{
		if (m_defaultDisplay)
			eglTerminate(m_defaultDisplay);
	}

	std::unique_ptr<Context> EGLLoader::CreateContext(const OpenGLDevice* device, const ContextParams& params, Context* shareContext) const
	{
		std::unique_ptr<EGLContextBase> context;
#ifdef NAZARA_PLATFORM_WINDOWS
		// On Windows context sharing seems to work only with window contexts
		context = std::make_unique<EGLContextWin32>(device, *this);
#else
		context = std::make_unique<EGLContextBase>(device, *this);
#endif

		if (!context->Create(params, static_cast<EGLContextBase*>(shareContext)))
		{
			NazaraError("failed to create context");
			return {};
		}

		if (!context->Initialize(params))
		{
			NazaraError("failed to initialize context");
			return {};
		}

		return context;
	}

	std::unique_ptr<Context> EGLLoader::CreateContext(const OpenGLDevice* device, const ContextParams& params, WindowHandle handle, Context* shareContext) const
	{
		std::unique_ptr<EGLContextBase> context;
		switch (handle.type)
		{
			case WindowManager::Invalid:
				break;

			case WindowManager::X11:
#ifdef NAZARA_PLATFORM_LINUX
				context = std::make_unique<EGLContextX11>(device, *this);
#endif
				break;

			case WindowManager::Windows:
#ifdef NAZARA_PLATFORM_WINDOWS
				context = std::make_unique<EGLContextWin32>(device, *this);
#endif
				break;

			case WindowManager::Wayland:
#ifdef NAZARA_PLATFORM_LINUX
				context = std::make_unique<EGLContextWayland>(device, *this);
#endif
				break;
		}

		if (!context)
		{
			NazaraError("unsupported window type");
			return {};
		}

		if (!context->Create(params, handle, static_cast<EGLContextBase*>(shareContext)))
		{
			NazaraError("failed to create context");
			return {};
		}

		if (!context->Initialize(params))
		{
			NazaraError("failed to initialize context");
			return {};
		}

		return context;
	}

	GLFunction EGLLoader::LoadFunction(const char* name) const
	{
		return eglGetProcAddress(name);
	}

	const char* EGLLoader::TranslateError(EGLint errorId)
	{
		switch (errorId)
		{
			case EGL_SUCCESS: return "The last function succeeded without error.";
			case EGL_NOT_INITIALIZED: return "EGL is not initialized, or could not be initialized, for the specified EGL display connection.";
			case EGL_BAD_ACCESS: return "EGL cannot access a requested resource.";
			case EGL_BAD_ALLOC: return "EGL failed to allocate resources for the requested operation.";
			case EGL_BAD_ATTRIBUTE: return "An unrecognized attribute or attribute value was passed in the attribute list.";
			case EGL_BAD_CONTEXT: return "An EGLContext argument does not name a valid EGL rendering context.";
			case EGL_BAD_CONFIG: return "An EGLConfig argument does not name a valid EGL frame buffer configuration.";
			case EGL_BAD_CURRENT_SURFACE: return "The current surface of the calling thread is a window, pixel buffer or pixmap that is no longer valid.";
			case EGL_BAD_DISPLAY: return "An EGLDisplay argument does not name a valid EGL display connection.";
			case EGL_BAD_SURFACE: return "An EGLSurface argument does not name a valid surface (window, pixel buffer or pixmap) configured for GL rendering.";
			case EGL_BAD_MATCH: return "Arguments are inconsistent.";
			case EGL_BAD_PARAMETER: return "One or more argument values are invalid.";
			case EGL_BAD_NATIVE_PIXMAP: return "A NativePixmapType argument does not refer to a valid native pixmap.";
			case EGL_BAD_NATIVE_WINDOW: return "A NativeWindowType argument does not refer to a valid native window.";
			case EGL_CONTEXT_LOST: return "A power management event has occurred.";
			default: return "Invalid or unknown error.";
		}
	}
}
