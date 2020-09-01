// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_EGLLOADER_HPP
#define NAZARA_OPENGLRENDERER_EGLLOADER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Loader.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLContextBase.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLFunctions.hpp>
#include <string>

namespace Nz::GL
{
	class NAZARA_OPENGLRENDERER_API EGLLoader : public Loader
	{
		public:
			EGLLoader(DynLib& openglLib);
			~EGLLoader() = default;

			std::unique_ptr<Context> CreateContext(const OpenGLDevice* device, const ContextParams& params, Context* shareContext) const override;
			std::unique_ptr<Context> CreateContext(const OpenGLDevice* device, const ContextParams& params, WindowHandle handle, Context* shareContext) const override;

			GLFunction LoadFunction(const char* name) const override;

#define NAZARA_OPENGLRENDERER_FUNC(name, sig) sig name = nullptr;
#define NAZARA_OPENGLRENDERER_EXT_BEGIN(ext)
#define NAZARA_OPENGLRENDERER_EXT_END()
#define NAZARA_OPENGLRENDERER_EXT_FUNC(name, sig)
			NAZARA_OPENGLRENDERER_FOREACH_EGL_FUNC(NAZARA_OPENGLRENDERER_FUNC, NAZARA_OPENGLRENDERER_EXT_BEGIN, NAZARA_OPENGLRENDERER_EXT_END, NAZARA_OPENGLRENDERER_EXT_FUNC)
#undef NAZARA_OPENGLRENDERER_EXT_BEGIN
#undef NAZARA_OPENGLRENDERER_EXT_END
#undef NAZARA_OPENGLRENDERER_EXT_FUNC
#undef NAZARA_OPENGLRENDERER_FUNC

			static const char* TranslateError(EGLint errorId);

		private:
			DynLib m_eglLib;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLLoader.inl>

#endif
