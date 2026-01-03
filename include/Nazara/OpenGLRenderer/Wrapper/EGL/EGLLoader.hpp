// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_EGL_EGLLOADER_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_EGL_EGLLOADER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Export.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Loader.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLContextBase.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <string>

namespace Nz::GL
{
	class NAZARA_OPENGLRENDERER_API EGLLoader : public Loader
	{
		struct SymbolLoader;
		friend SymbolLoader;

		public:
			EGLLoader(const Renderer::Config& config);
			~EGLLoader();

			std::shared_ptr<Context> CreateContext(const OpenGLDevice* device, const ContextParams& params, Context* shareContext) const override;
			std::shared_ptr<Context> CreateContext(const OpenGLDevice* device, const ContextParams& params, WindowHandle handle, Context* shareContext) const override;

			inline EGLDisplay GetDefaultDisplay() const;
			ContextType GetPreferredContextType() const override;

			GLFunction LoadFunction(const char* name) const override;

#define NAZARA_OPENGLRENDERER_EGL_FUNC(name, sig) sig name = nullptr;
#define NAZARA_OPENGLRENDERER_EGL_FUNC_OPT(name, sig) NAZARA_OPENGLRENDERER_EGL_FUNC(name, sig)

#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLFunctions.hpp>

			static const char* TranslateError(EGLint errorId);

		private:
			bool ImplementFallback(std::string_view function);

			ContextType m_preferredContextType;
			EGLDisplay m_defaultDisplay;
			DynLib m_eglLib;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLLoader.inl>

#endif // NAZARA_OPENGLRENDERER_WRAPPER_EGL_EGLLOADER_HPP
