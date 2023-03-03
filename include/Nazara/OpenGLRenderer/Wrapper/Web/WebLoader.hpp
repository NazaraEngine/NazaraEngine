// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_WEB_WEBLOADER_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_WEB_WEBLOADER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Loader.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Web/WebContext.hpp>
#include <string>

namespace Nz::GL
{
	class NAZARA_OPENGLRENDERER_API WebLoader : public Loader
	{
		struct SymbolLoader;
		friend SymbolLoader;

		public:
			std::shared_ptr<Context> CreateContext(const OpenGLDevice* device, const ContextParams& params, Context* shareContext) const override;
			std::shared_ptr<Context> CreateContext(const OpenGLDevice* device, const ContextParams& params, WindowHandle handle, Context* shareContext) const override;

			ContextType GetPreferredContextType() const override;

			GLFunction LoadFunction(const char* name) const override;

			static const char* TranslateError(EMSCRIPTEN_RESULT errorId);

		private:
			bool ImplementFallback(const std::string_view& function);
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Web/WebLoader.inl>

#endif // NAZARA_OPENGLRENDERER_WRAPPER_WEB_WEBLOADER_HPP