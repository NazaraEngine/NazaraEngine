// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WGLLOADER_HPP
#define NAZARA_OPENGLRENDERER_WGLLOADER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Loader.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/WGL/WGLContext.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/WGL/WGLFunctions.hpp>
#include <string>

namespace Nz::GL
{
	class NAZARA_OPENGLRENDERER_API WGLLoader : public Loader
	{
		public:
			WGLLoader();
			~WGLLoader() = default;

			std::unique_ptr<Context> CreateContext(const OpenGLDevice* device, const ContextParams& params, Context* shareContext) const override;
			std::unique_ptr<Context> CreateContext(const OpenGLDevice* device, const ContextParams& params, WindowHandle handle, Context* shareContext) const override;

			GLFunction LoadFunction(const char* name) const override;

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
			DynLib m_opengl32Lib;
			WGLContext m_baseContext;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/WGL/WGLLoader.inl>

#endif
