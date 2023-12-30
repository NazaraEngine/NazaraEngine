// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_WGL_WGLCONTEXT_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_WGL_WGLCONTEXT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/WGL/WGLFunctions.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Win32/Win32Helper.hpp>
#include <Nazara/Platform/WindowHandle.hpp>
#include <NazaraUtils/StringHash.hpp>
#include <string>
#include <type_traits>
#include <unordered_set>

namespace Nz::GL
{
	class WGLLoader;

	class NAZARA_OPENGLRENDERER_API WGLContext : public Context
	{
		public:
			inline WGLContext(const OpenGLDevice* device, const WGLLoader& loader);
			WGLContext(const WGLContext&) = delete;
			WGLContext(WGLContext&&) = delete;
			~WGLContext();

			bool Create(const WGLContext* baseContext, const ContextParams& params, const WGLContext* shareContext = nullptr);
			bool Create(const WGLContext* baseContext, const ContextParams& params, WindowHandle window, const WGLContext* shareContext = nullptr);
			void Destroy();

			PresentModeFlags GetSupportedPresentModes() const override;

			inline bool HasPlatformExtension(std::string_view str) const;

			void SetPresentMode(PresentMode presentMode) override;

			void SwapBuffers() override;

			WGLContext& operator=(const WGLContext&) = delete;
			WGLContext& operator=(WGLContext&&) = delete;

		private:
			bool CreateInternal(const WGLContext* baseContext, const ContextParams& params, const WGLContext* shareContext = nullptr);
			bool ImplementFallback(std::string_view function) override;

			bool Activate() const override;
			void Desactivate() const override;
			const Loader& GetLoader() override;
			bool LoadWGLExt();
			bool SetPixelFormat();

#define NAZARA_OPENGLRENDERER_FUNC(name, sig)
#define NAZARA_OPENGLRENDERER_EXT_BEGIN(ext)
#define NAZARA_OPENGLRENDERER_EXT_END()
#define NAZARA_OPENGLRENDERER_EXT_FUNC(name, sig) sig name = nullptr;
			NAZARA_OPENGLRENDERER_FOREACH_WGL_FUNC(NAZARA_OPENGLRENDERER_FUNC, NAZARA_OPENGLRENDERER_EXT_BEGIN, NAZARA_OPENGLRENDERER_EXT_END, NAZARA_OPENGLRENDERER_EXT_FUNC)
#undef NAZARA_OPENGLRENDERER_EXT_BEGIN
#undef NAZARA_OPENGLRENDERER_EXT_END
#undef NAZARA_OPENGLRENDERER_EXT_FUNC
#undef NAZARA_OPENGLRENDERER_FUNC

			struct Fallback
			{
				using glClearDepthProc = void(*)(double depth);

				glClearDepthProc glClearDepth;
			};
			Fallback fallbacks; //< m_ omitted

			std::unordered_set<std::string, StringHash<>, std::equal_to<>> m_supportedPlatformExtensions;
			const WGLLoader& m_loader;
			HDC m_deviceContext;
			HGLRC m_handle;
			HWNDHandle m_window;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/WGL/WGLContext.inl>

#endif // NAZARA_OPENGLRENDERER_WRAPPER_WGL_WGLCONTEXT_HPP
