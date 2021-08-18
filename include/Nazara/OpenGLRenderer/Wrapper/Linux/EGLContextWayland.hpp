// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_EGLCONTEXTWAYLAND_HPP
#define NAZARA_OPENGLRENDERER_EGLCONTEXTWAYLAND_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLContextBase.hpp>

namespace Nz::GL
{
	class EGLContextWayland final : public EGLContextBase
	{
		public:
			using EGLContextBase::EGLContextBase;
			EGLContextWayland(const EGLContextWayland&) = default;
			EGLContextWayland(EGLContextWayland&&) = default;
			~EGLContextWayland() = default;

			bool Create(const ContextParams& params, WindowHandle window, const EGLContextBase* shareContext = nullptr) override;
			void Destroy() override;

			EGLContextWayland& operator=(const EGLContextWayland&) = default;
			EGLContextWayland& operator=(EGLContextWayland&&) = default;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Linux/EGLContextWayland.inl>

#endif
