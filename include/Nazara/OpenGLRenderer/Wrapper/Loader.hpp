// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_LOADER_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_LOADER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Export.hpp>
#include <Nazara/Platform/WindowHandle.hpp>
#include <memory>

namespace Nz
{
	class OpenGLDevice;
}

namespace Nz::GL
{
	using GLFunction = void(*)(void);

	class Context;
	enum class ContextType;
	struct ContextParams;

	class NAZARA_OPENGLRENDERER_API Loader
	{
		public:
			Loader() = default;
			virtual ~Loader();

			virtual std::shared_ptr<Context> CreateContext(const OpenGLDevice* device, const ContextParams& params, Context* shareContext = nullptr) const = 0;
			virtual std::shared_ptr<Context> CreateContext(const OpenGLDevice* device, const ContextParams& params, WindowHandle handle, Context* shareContext = nullptr) const = 0;

			virtual ContextType GetPreferredContextType() const = 0;

			virtual GLFunction LoadFunction(const char* name) const = 0;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Loader.inl>

#endif // NAZARA_OPENGLRENDERER_WRAPPER_LOADER_HPP
