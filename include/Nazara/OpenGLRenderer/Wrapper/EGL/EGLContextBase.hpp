// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_EGL_EGLCONTEXTBASE_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_EGL_EGLCONTEXTBASE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Platform/WindowHandle.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <string>
#include <type_traits>
#include <unordered_set>

namespace Nz::GL
{
	class EGLLoader;

	// This class isn't called EGLContext to prevent a conflict with EGLContext from EGL
	class NAZARA_OPENGLRENDERER_API EGLContextBase : public Context
	{
		public:
			inline EGLContextBase(const OpenGLDevice* device, const EGLLoader& loader);
			EGLContextBase(const EGLContextBase&) = delete;
			EGLContextBase(EGLContextBase&&) = delete;
			~EGLContextBase();

			virtual bool Create(const ContextParams& params, const EGLContextBase* shareContext = nullptr);
			virtual bool Create(const ContextParams& params, WindowHandle window, const EGLContextBase* shareContext = nullptr);
			virtual void Destroy();

			void EnableVerticalSync(bool enabled) override;

			inline bool HasPlatformExtension(const std::string& str) const;

			void SwapBuffers() override;

			EGLContextBase& operator=(const EGLContextBase&) = delete;
			EGLContextBase& operator=(EGLContextBase&&) = delete;

		protected:
			bool BindAPI();
			bool ChooseConfig(EGLConfig* configs, std::size_t maxConfigCount, std::size_t* configCount);
			bool CreateInternal(EGLConfig config, const EGLContextBase* shareContext = nullptr);
			bool InitDisplay();

			const EGLLoader& m_loader;
			EGLDisplay m_display;
			EGLSurface m_surface;

		private:
			bool ImplementFallback(const std::string_view& function) override;

			bool Activate() const override;
			void Desactivate() const override;
			const Loader& GetLoader() override;
			bool LoadEGLExt();

			struct Fallback
			{
				using glClearDepthProc = void(*)(double depth);

				glClearDepthProc glClearDepth;
			};
			Fallback fallbacks; //< m_ omitted

			std::unordered_set<std::string> m_supportedPlatformExtensions;
			EGLContext m_handle;
			bool m_ownsDisplay;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLContextBase.inl>

#endif // NAZARA_OPENGLRENDERER_WRAPPER_EGL_EGLCONTEXTBASE_HPP
