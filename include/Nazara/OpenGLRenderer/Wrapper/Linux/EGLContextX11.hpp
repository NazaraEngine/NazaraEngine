// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_LINUX_EGLCONTEXTX11_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_LINUX_EGLCONTEXTX11_HPP

#include <NazaraUtils/Prerequisites.hpp>
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

#endif // NAZARA_OPENGLRENDERER_WRAPPER_LINUX_EGLCONTEXTX11_HPP
