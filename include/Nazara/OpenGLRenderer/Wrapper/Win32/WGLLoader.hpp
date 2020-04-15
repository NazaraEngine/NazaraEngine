// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WGLLOADER_HPP
#define NAZARA_OPENGLRENDERER_WGLLOADER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Loader.hpp>
#include <string>

#undef WIN32_LEAN_AND_MEAN //< Redefined by OpenGL header (ty Khronos)
#include <Nazara/OpenGLRenderer/Wrapper/Win32/WGLFunctions.hpp>

namespace Nz::GL
{
	class WGLLoader : public Loader
	{
		public:
			WGLLoader(DynLib& openglLib);
			~WGLLoader() = default;

			std::unique_ptr<GLContext> CreateContext() override;

			GLFunction LoadFunction(const char* name) override;

#define NAZARA_OPENGLRENDERER_FUNC(name, sig) sig name = nullptr;
#define NAZARA_OPENGLRENDERER_EXT_BEGIN(ext)
#define NAZARA_OPENGLRENDERER_EXT_END()
#define NAZARA_OPENGLRENDERER_EXT_FUNC(name, sig)
			NAZARA_OPENGLRENDERER_FOREACH_GDI32_FUNC(NAZARA_OPENGLRENDERER_FUNC)
			NAZARA_OPENGLRENDERER_FOREACH_WGL_FUNC(NAZARA_OPENGLRENDERER_FUNC, NAZARA_OPENGLRENDERER_EXT_BEGIN, NAZARA_OPENGLRENDERER_EXT_END, NAZARA_OPENGLRENDERER_EXT_FUNC)
#undef NAZARA_OPENGLRENDERER_EXT_BEGIN
#undef NAZARA_OPENGLRENDERER_EXT_END
#undef NAZARA_OPENGLRENDERER_EXT_FUNC
#undef NAZARA_OPENGLRENDERER_FUNC

		private:
			DynLib m_gdi32Lib;
			DynLib& m_opengl32Lib;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Win32/WGLLoader.inl>

#endif
