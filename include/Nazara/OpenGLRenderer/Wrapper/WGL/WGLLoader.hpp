// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_WGL_WGLLOADER_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_WGL_WGLLOADER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/OpenGLRenderer/Export.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Loader.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/WGL/WGLContext.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/WGL/WGLFunctions.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <string>

namespace Nz::GL
{
	class NAZARA_OPENGLRENDERER_API WGLLoader : public Loader
	{
		public:
			WGLLoader(const Renderer::Config& config);
			~WGLLoader() = default;

			std::shared_ptr<Context> CreateContext(const OpenGLDevice* device, const ContextParams& params, Context* shareContext) const override;
			std::shared_ptr<Context> CreateContext(const OpenGLDevice* device, const ContextParams& params, WindowHandle handle, Context* shareContext) const override;

			ContextType GetPreferredContextType() const override;

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

#endif // NAZARA_OPENGLRENDERER_WRAPPER_WGL_WGLLOADER_HPP
