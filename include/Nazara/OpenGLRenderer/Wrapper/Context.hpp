// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_CONTEXTIMPL_HPP
#define NAZARA_OPENGLRENDERER_CONTEXTIMPL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/CoreFunctions.hpp>
#include <string>
#include <unordered_set>

namespace Nz::GL
{
	enum class ContextType
	{
		OpenGL,
		OpenGL_ES
	};

	struct ContextParams
	{
		ContextType type = ContextType::OpenGL_ES;
		bool doubleBuffering = true;
		unsigned int bitsPerPixel = 32;
		unsigned int depthBits = 24;
		unsigned int glMajorVersion = 0;
		unsigned int glMinorVersion = 0;
		unsigned int sampleCount = 1;
		unsigned int stencilBits = 8;
	};

	class Loader;

	class Context
	{
		public:
			Context() = default;
			virtual ~Context();

			virtual bool Activate() = 0;

			virtual void EnableVerticalSync(bool enabled) = 0;

			inline const ContextParams& GetParams() const;

			inline bool IsExtensionSupported(const std::string& extension) const;

			bool Initialize(const ContextParams& params);

			virtual void SwapBuffers() = 0;

#define NAZARA_OPENGLRENDERER_FUNC(name, sig) sig name = nullptr;
			NAZARA_OPENGLRENDERER_FOREACH_GLES_FUNC(NAZARA_OPENGLRENDERER_FUNC)
#undef NAZARA_OPENGLRENDERER_FUNC

		protected:
			virtual const Loader& GetLoader() = 0;

			virtual bool ImplementFallback(const std::string_view& function) = 0;

			std::unordered_set<std::string> m_supportedExtensions;
			ContextParams m_params;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Context.inl>

#endif
