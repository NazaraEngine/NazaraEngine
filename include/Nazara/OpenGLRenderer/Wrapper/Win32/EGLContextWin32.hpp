// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_EGLCONTEXTWIN32_HPP
#define NAZARA_OPENGLRENDERER_EGLCONTEXTWIN32_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLContextBase.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Win32/Win32Helper.hpp>

namespace Nz::GL
{
	class EGLContextWin32 final : public EGLContextBase
	{
		public:
			using EGLContextBase::EGLContextBase;
			EGLContextWin32(const EGLContextWin32&) = default;
			EGLContextWin32(EGLContextWin32&&) = default;
			~EGLContextWin32() = default;

			bool Create(const ContextParams& params, const EGLContextBase* shareContext = nullptr);
			bool Create(const ContextParams& params, WindowHandle window, const EGLContextBase* shareContext = nullptr) override;
			void Destroy() override;

			EGLContextWin32& operator=(const EGLContextWin32&) = default;
			EGLContextWin32& operator=(EGLContextWin32&&) = default;

		private:
			HWNDHandle m_ownedWindow;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Win32/EGLContextWin32.inl>

#endif
