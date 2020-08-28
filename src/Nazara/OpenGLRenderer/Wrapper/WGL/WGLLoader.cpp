// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/WGL/WGLLoader.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/WGL/WGLContext.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	WGLLoader::WGLLoader(DynLib& openglLib) :
	m_opengl32Lib(openglLib),
	m_baseContext(nullptr, *this)
	{
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

		if (!m_baseContext.Create(&loadContext, params))
			throw std::runtime_error("failed to create load context");

		if (!m_baseContext.Initialize(params))
			throw std::runtime_error("failed to load OpenGL functions");
	}

	std::unique_ptr<Context> WGLLoader::CreateContext(const OpenGLDevice* device, const ContextParams& params, Context* shareContext) const
	{
		auto context = std::make_unique<WGLContext>(device, *this);
		if (!context->Create(&m_baseContext, params, static_cast<WGLContext*>(shareContext)))
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

	std::unique_ptr<Context> WGLLoader::CreateContext(const OpenGLDevice* device, const ContextParams& params, WindowHandle handle, Context* shareContext) const
	{
		auto context = std::make_unique<WGLContext>(device, *this);
		if (!context->Create(&m_baseContext, params, handle, static_cast<WGLContext*>(shareContext)))
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

	GLFunction WGLLoader::LoadFunction(const char* name) const
	{
		GLFunction func = reinterpret_cast<GLFunction>(wglGetProcAddress(name));
		if (!func) //< wglGetProcAddress doesn't work for OpenGL 1.1 functions
			func = reinterpret_cast<GLFunction>(m_opengl32Lib.GetSymbol(name));

		return func;
	}
}
