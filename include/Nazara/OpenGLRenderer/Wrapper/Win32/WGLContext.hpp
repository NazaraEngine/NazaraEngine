// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WGLCONTEXT_HPP
#define NAZARA_OPENGLRENDERER_WGLCONTEXT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/GLContext.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Win32/WGLLoader.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Win32/Win32Helper.hpp>
#include <string>
#include <type_traits>
#include <unordered_set>

#undef WIN32_LEAN_AND_MEAN //< Redefined by OpenGL header (ty Khronos)
#include <Nazara/OpenGLRenderer/Wrapper/Win32/WGLFunctions.hpp>

namespace Nz::GL
{
	class WGLLoader;

	class WGLContext : public GLContext
	{
		public:
			WGLContext(WGLLoader& loader);
			WGLContext(const WGLContext&) = delete;
			WGLContext(WGLContext&&) = delete;
			~WGLContext();

			bool Activate() override;

			bool Create(const ContextParams& params) override;
			void Destroy();

			void EnableVerticalSync(bool enabled) override;

			void SwapBuffers() override;

			WGLContext& operator=(const WGLContext&) = delete;
			WGLContext& operator=(WGLContext&&) = delete;

		private:
			void Desactivate();
			bool LoadWGLExt();
			bool SetPixelFormat(const ContextParams& params);

#define NAZARA_OPENGLRENDERER_FUNC(name, sig)
#define NAZARA_OPENGLRENDERER_EXT_BEGIN(ext)
#define NAZARA_OPENGLRENDERER_EXT_END()
#define NAZARA_OPENGLRENDERER_EXT_FUNC(name, sig) sig name = nullptr;
			NAZARA_OPENGLRENDERER_FOREACH_WGL_FUNC(NAZARA_OPENGLRENDERER_FUNC, NAZARA_OPENGLRENDERER_EXT_BEGIN, NAZARA_OPENGLRENDERER_EXT_END, NAZARA_OPENGLRENDERER_EXT_FUNC)
#undef NAZARA_OPENGLRENDERER_EXT_BEGIN
#undef NAZARA_OPENGLRENDERER_EXT_END
#undef NAZARA_OPENGLRENDERER_EXT_FUNC
#undef NAZARA_OPENGLRENDERER_FUNC

			std::unordered_set<std::string> m_supportedExtensions;
			WGLLoader& m_loader;
			HDC m_deviceContext;
			HGLRC m_handle;
			HWNDHandle m_window;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Win32/WGLContext.inl>

#endif
