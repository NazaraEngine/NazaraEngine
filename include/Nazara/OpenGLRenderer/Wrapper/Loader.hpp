// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_LOADER_HPP
#define NAZARA_OPENGLRENDERER_LOADER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <memory>

namespace Nz::GL
{
	class GLContext;

	using GLFunction = int(*)();

	class NAZARA_OPENGLRENDERER_API Loader
	{
		public:
			Loader() = default;
			virtual ~Loader();

			virtual std::unique_ptr<GLContext> CreateContext() = 0;

			virtual GLFunction LoadFunction(const char* name) = 0;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Loader.inl>

#endif
