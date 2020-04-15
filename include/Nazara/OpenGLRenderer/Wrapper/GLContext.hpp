// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_CONTEXTIMPL_HPP
#define NAZARA_OPENGLRENDERER_CONTEXTIMPL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/CoreFunctions.hpp>
#include <string>

namespace Nz::GL
{
	struct ContextParams
	{
		bool doubleBuffering = true;
		unsigned int sampleCount = 1;
		unsigned int bitsPerPixel = 32;
		unsigned int depthBits = 24;
		unsigned int stencilBits = 8;
	};

	class Loader;

	class GLContext
	{
		public:
			GLContext() = default;
			virtual ~GLContext();

			virtual bool Activate() = 0;

			virtual bool Create(const ContextParams& params) = 0;

			virtual void EnableVerticalSync(bool enabled) = 0;

			bool LoadCoreFunctions(Loader& loader);

			virtual void SwapBuffers() = 0;

		private:
#define NAZARA_OPENGLRENDERER_FUNC(name, sig) sig name = nullptr;
			NAZARA_OPENGLRENDERER_FOREACH_GLES_FUNC(NAZARA_OPENGLRENDERER_FUNC)
#undef NAZARA_OPENGLRENDERER_FUNC
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/GLContext.inl>

#endif
