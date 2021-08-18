// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_LOADER_HPP
#define NAZARA_OPENGLRENDERER_LOADER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Platform/WindowHandle.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <memory>

namespace Nz
{
	class OpenGLDevice;
}

namespace Nz::GL
{
	using GLFunction = void(*)(void);

	class Context;
	struct ContextParams;

	class NAZARA_OPENGLRENDERER_API Loader
	{
		public:
			Loader() = default;
			virtual ~Loader();

			virtual std::unique_ptr<Context> CreateContext(const OpenGLDevice* device, const ContextParams& params, Context* shareContext = nullptr) const = 0;
			virtual std::unique_ptr<Context> CreateContext(const OpenGLDevice* device, const ContextParams& params, WindowHandle handle, Context* shareContext = nullptr) const = 0;

			virtual GLFunction LoadFunction(const char* name) const = 0;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Loader.inl>

#endif
