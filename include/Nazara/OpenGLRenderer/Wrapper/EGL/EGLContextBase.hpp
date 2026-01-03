// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_EGL_EGLCONTEXTBASE_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_EGL_EGLCONTEXTBASE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/OpenGLRenderer/Export.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/Platform/WindowHandle.hpp>
#include <NazaraUtils/StringHash.hpp>
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

			PresentModeFlags GetSupportedPresentModes() const override;

			inline bool HasPlatformExtension(std::string_view str) const;

			void SetPresentMode(PresentMode presentMode) override;

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
			bool ImplementFallback(std::string_view function) override;

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

			std::unordered_set<std::string, StringHash<>, std::equal_to<>> m_supportedPlatformExtensions;
			EGLContext m_handle;
			EGLint m_maxSwapInterval;
			EGLint m_minSwapInterval;
			bool m_ownsDisplay;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLContextBase.inl>

#endif // NAZARA_OPENGLRENDERER_WRAPPER_EGL_EGLCONTEXTBASE_HPP
