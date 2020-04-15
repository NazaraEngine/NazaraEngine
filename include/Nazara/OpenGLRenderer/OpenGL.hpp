// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGL_HPP
#define NAZARA_OPENGL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Initializer.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <list>
#include <memory>
#include <vector>

namespace Nz
{
	class OpenGLDevice;

	class NAZARA_OPENGLRENDERER_API OpenGL
	{
		public:
			OpenGL() = delete;
			~OpenGL() = delete;

			static bool Initialize();

			static bool IsInitialized();

			static void Uninitialize();
	};	
}

#endif // NAZARA_OPENGL_HPP
