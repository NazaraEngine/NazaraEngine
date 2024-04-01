// Copyright (C) 2025 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_ANDROID_EGLCONTEXTANDROID_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_ANDROID_EGLCONTEXTANDROID_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLContextBase.hpp>

namespace Nz::GL
{
	class EGLContextAndroid final : public EGLContextBase
	{
		public:
			using EGLContextBase::EGLContextBase;
			EGLContextAndroid(const EGLContextAndroid&) = delete;
			EGLContextAndroid(EGLContextAndroid&&) = delete;
			~EGLContextAndroid() = default;

			bool Create(const ContextParams& params, WindowHandle window, const EGLContextBase* shareContext = nullptr) override;

			EGLContextAndroid& operator=(const EGLContextAndroid&) = delete;
			EGLContextAndroid& operator=(EGLContextAndroid&&) = delete;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Android/EGLContextAndroid.inl>

#endif // NAZARA_OPENGLRENDERER_WRAPPER_ANDROID_EGLCONTEXTANDROID_HPP
