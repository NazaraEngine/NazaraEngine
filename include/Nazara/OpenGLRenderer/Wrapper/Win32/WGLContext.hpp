// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WGLCONTEXT_HPP
#define NAZARA_OPENGLRENDERER_WGLCONTEXT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Platform/WindowHandle.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Win32/Win32Helper.hpp>
#include <string>
#include <type_traits>
#include <unordered_set>

#undef WIN32_LEAN_AND_MEAN //< Redefined by OpenGL header (ty Khronos)
#include <Nazara/OpenGLRenderer/Wrapper/Win32/WGLFunctions.hpp>

namespace Nz::GL
{
	class WGLLoader;

	class WGLContext : public Context
	{
		public:
			WGLContext(const WGLLoader& loader);
			WGLContext(const WGLContext&) = delete;
			WGLContext(WGLContext&&) = delete;
			~WGLContext();

			bool Activate() override;

			bool Create(const WGLContext* baseContext, const ContextParams& params, const WGLContext* shareContext = nullptr);
			bool Create(const WGLContext* baseContext, const ContextParams& params, WindowHandle window, const WGLContext* shareContext = nullptr);
			void Destroy();

			void EnableVerticalSync(bool enabled) override;

			inline bool HasPlatformExtension(const std::string& str) const;

			void SwapBuffers() override;

			WGLContext& operator=(const WGLContext&) = delete;
			WGLContext& operator=(WGLContext&&) = delete;

		private:
			bool CreateInternal(const WGLContext* baseContext, const ContextParams& params, const WGLContext* shareContext = nullptr);
			bool ImplementFallback(const std::string_view& function) override;

			void Desactivate();
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

			std::unordered_set<std::string> m_supportedPlatformExtensions;
			const WGLLoader& m_loader;
			HDC m_deviceContext;
			HGLRC m_handle;
			HWNDHandle m_window;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Win32/WGLContext.inl>

#endif
