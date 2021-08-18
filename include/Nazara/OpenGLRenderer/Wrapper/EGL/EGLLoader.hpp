// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_EGLLOADER_HPP
#define NAZARA_OPENGLRENDERER_EGLLOADER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Loader.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLContextBase.hpp>
#include <string>

namespace Nz::GL
{
	class NAZARA_OPENGLRENDERER_API EGLLoader : public Loader
	{
		struct SymbolLoader;
		friend SymbolLoader;

		public:
			EGLLoader();
			~EGLLoader();

			std::unique_ptr<Context> CreateContext(const OpenGLDevice* device, const ContextParams& params, Context* shareContext) const override;
			std::unique_ptr<Context> CreateContext(const OpenGLDevice* device, const ContextParams& params, WindowHandle handle, Context* shareContext) const override;

			inline EGLDisplay GetDefaultDisplay() const;

			GLFunction LoadFunction(const char* name) const override;

#define NAZARA_OPENGLRENDERER_EGL_FUNC(name, sig) sig name = nullptr;
#define NAZARA_OPENGLRENDERER_EGL_FUNC_OPT(name, sig) NAZARA_OPENGLRENDERER_EGL_FUNC(name, sig)

#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLFunctions.hpp>

			static const char* TranslateError(EGLint errorId);

		private:
			bool ImplementFallback(const std::string_view& function);

			EGLDisplay m_defaultDisplay;
			DynLib m_eglLib;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLLoader.inl>

#endif
