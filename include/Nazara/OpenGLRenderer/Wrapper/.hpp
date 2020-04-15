// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_GLCONTEXT_HPP
#define NAZARA_OPENGLRENDERER_GLCONTEXT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/CoreFunctions.hpp>
#include <string>

namespace Nz::GL
{
	struct ContextParams
	{

	};

	class GLContext
	{
		public:
			GLContext() = default;
			virtual ~GLContext();

			virtual bool Activate() = 0;

			virtual bool Create(const ContextParams& params) = 0;

			virtual void EnableVerticalSync(bool enabled) = 0;

			virtual void SwapBuffers() = 0;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/GLContext.inl>

#endif
