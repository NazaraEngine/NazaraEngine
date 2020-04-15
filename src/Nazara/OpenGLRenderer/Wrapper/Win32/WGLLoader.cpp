// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Win32/WGLLoader.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Win32/WGLContext.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	WGLLoader::WGLLoader(DynLib& openglLib) :
	m_opengl32Lib(openglLib)
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

		// In order to load OpenGL functions, we have to create a context first
		WGLContext dummyContext(*this);

		if (!dummyContext.Create({}))
			throw std::runtime_error("failed to create load context");

		WGLContext loadContext(*this);

		if (!loadContext.Create({}))
			throw std::runtime_error("failed to create load context");

		if (!loadContext.LoadCoreFunctions(*this))
			throw std::runtime_error("failed to load OpenGL functions");

#undef NAZARA_OPENGLRENDERER_EXT_BEGIN
#undef NAZARA_OPENGLRENDERER_EXT_END
#undef NAZARA_OPENGLRENDERER_EXT_FUNC
	}

	std::unique_ptr<GLContext> WGLLoader::CreateContext()
	{
		return {}; //< TODO
	}

	GLFunction WGLLoader::LoadFunction(const char* name)
	{
		GLFunction func = reinterpret_cast<GLFunction>(wglGetProcAddress(name));
		if (!func) //< wglGetProcAddress doesn't work for OpenGL 1.1 functions
			func = reinterpret_cast<GLFunction>(m_opengl32Lib.GetSymbol(name));

		return func;
	}
}
