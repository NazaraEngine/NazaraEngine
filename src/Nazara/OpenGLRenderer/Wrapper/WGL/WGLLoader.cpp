// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/OpenGLRenderer/Wrapper/WGL/WGLLoader.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/WGL/WGLContext.hpp>

namespace Nz::GL
{
	WGLLoader::WGLLoader(const Renderer::Config& config) :
	m_baseContext(nullptr, *this)
	{
		if (!m_opengl32Lib.Load("opengl32.dll"))
			throw std::runtime_error("Failed to load opengl32 library, is OpenGL installed on your system?");

		if (!m_gdi32Lib.Load("gdi32.dll"))
			throw std::runtime_error("failed to load gdi32.dll: " + m_gdi32Lib.GetLastError());

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

#define NAZARA_OPENGLRENDERER_FUNC(name, sig) LoadSymbol(m_gdi32Lib, name, #name);
		NAZARA_OPENGLRENDERER_FOREACH_GDI32_FUNC(NAZARA_OPENGLRENDERER_FUNC)
#undef NAZARA_OPENGLRENDERER_FUNC

		// Load base WGL functions
#define NAZARA_OPENGLRENDERER_FUNC(name, sig) LoadSymbol(m_opengl32Lib, name, #name);
		NAZARA_OPENGLRENDERER_FOREACH_WGL_FUNC(NAZARA_OPENGLRENDERER_FUNC, NAZARA_OPENGLRENDERER_EXT_BEGIN, NAZARA_OPENGLRENDERER_EXT_END, NAZARA_OPENGLRENDERER_EXT_FUNC)
#undef NAZARA_OPENGLRENDERER_FUNC

#undef NAZARA_OPENGLRENDERER_EXT_BEGIN
#undef NAZARA_OPENGLRENDERER_EXT_END
#undef NAZARA_OPENGLRENDERER_EXT_FUNC

		// In order to load OpenGL functions, we have to create a context first
		WGLContext loadContext(nullptr, *this);

		if (!loadContext.Create(nullptr, {}))
			throw std::runtime_error("failed to create load context");

		ContextParams params;
		std::array<GL::ContextType, 2> contextTypes;

		RenderAPI preferredAPI = config.preferredAPI;
		if (preferredAPI == RenderAPI::Unknown)
		{
			// Favor OpenGL on desktop and OpenGL ES on mobile
#if defined(NAZARA_PLATFORM_DESKTOP)
			preferredAPI = RenderAPI::OpenGL;
#else
			preferredAPI = RenderAPI::OpenGL_ES;
#endif
		}

		if (config.preferredAPI == RenderAPI::OpenGL_ES)
		{
			contextTypes[0] = GL::ContextType::OpenGL_ES;
			contextTypes[1] = GL::ContextType::OpenGL;
		}
		else
		{
			contextTypes[0] = GL::ContextType::OpenGL;
			contextTypes[1] = GL::ContextType::OpenGL_ES;
		}

		bool created = false;
		for (GL::ContextType contextType : contextTypes)
		{
			params.type = contextType;

			if (!m_baseContext.Create(&loadContext, params) || m_baseContext.GetParams().type != contextType)
				continue;

			if (!m_baseContext.Initialize(params))
				continue;

			created = true;
			break;
		}

		if (!created)
			throw std::runtime_error("failed to create or initialize base context");
	}

	std::shared_ptr<Context> WGLLoader::CreateContext(const OpenGLDevice* device, const ContextParams& params, Context* shareContext) const
	{
		auto context = std::make_shared<WGLContext>(device, *this);
		if (!context->Create(&m_baseContext, params, SafeCast<WGLContext*>(shareContext)))
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

	std::shared_ptr<Context> WGLLoader::CreateContext(const OpenGLDevice* device, const ContextParams& params, WindowHandle handle, Context* shareContext) const
	{
		auto context = std::make_shared<WGLContext>(device, *this);
		if (!context->Create(&m_baseContext, params, handle, SafeCast<WGLContext*>(shareContext)))
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

	ContextType WGLLoader::GetPreferredContextType() const
	{
		return m_baseContext.GetParams().type;
	}

	GLFunction WGLLoader::LoadFunction(const char* name) const
	{
		GLFunction func = reinterpret_cast<GLFunction>(wglGetProcAddress(name));
		if (!func) //< wglGetProcAddress doesn't work for OpenGL 1.1 functions
			func = reinterpret_cast<GLFunction>(m_opengl32Lib.GetSymbol(name));

		return func;
	}
}

#include <Nazara/Core/AntiWindows.hpp>
