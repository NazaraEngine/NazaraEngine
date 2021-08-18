// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_EGLCONTEXTWIN32_HPP
#define NAZARA_OPENGLRENDERER_EGLCONTEXTWIN32_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLContextBase.hpp>

namespace Nz::GL
{
	class EGLContextX11 final : public EGLContextBase
	{
		public:
			using EGLContextBase::EGLContextBase;
			EGLContextX11(const EGLContextX11&) = delete;
			EGLContextX11(EGLContextX11&&) = delete;
			~EGLContextX11() = default;

			bool Create(const ContextParams& params, WindowHandle window, const EGLContextBase* shareContext = nullptr) override;

			EGLContextX11& operator=(const EGLContextX11&) = delete;
			EGLContextX11& operator=(EGLContextX11&&) = delete;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Linux/EGLContextX11.inl>

#endif
