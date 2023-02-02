// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_WEB_WEBCONTEXT_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_WEB_WEBCONTEXT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/Platform/WindowHandle.hpp>
#include <emscripten/html5.h>
#include <string>
#include <type_traits>
#include <unordered_set>

namespace Nz::GL
{
	class WebLoader;

	class NAZARA_OPENGLRENDERER_API WebContext : public Context
	{
		public:
			inline WebContext(const OpenGLDevice* device, const WebLoader& loader);
			WebContext(const WebContext&) = delete;
			WebContext(WebContext&&) = delete;
			~WebContext();

			virtual bool Create(const ContextParams& params, const WebContext* shareContext = nullptr);
			virtual bool Create(const ContextParams& params, WindowHandle window, const WebContext* shareContext = nullptr);
			virtual void Destroy();

			void EnableVerticalSync(bool enabled) override;

			inline bool HasPlatformExtension(const std::string& str) const;

			void SwapBuffers() override;

			WebContext& operator=(const WebContext&) = delete;
			WebContext& operator=(WebContext&&) = delete;

		protected:
			bool ChooseConfig(EmscriptenWebGLContextAttributes* configs, std::size_t maxConfigCount, std::size_t* configCount);
			bool CreateInternal(EmscriptenWebGLContextAttributes config, const WebContext* shareContext = nullptr);

			const WebLoader& m_loader;

		private:
			bool ImplementFallback(const std::string_view& function) override;

			bool Activate() const override;
			void Desactivate() const override;
			const Loader& GetLoader() override;
			bool LoadExt();

			struct Fallback
			{
				using glClearDepthProc = void(*)(double depth);

				glClearDepthProc glClearDepth;
			};
			Fallback fallbacks; //< m_ omitted

			std::unordered_set<std::string> m_supportedPlatformExtensions;
			EMSCRIPTEN_WEBGL_CONTEXT_HANDLE m_handle;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Web/WebContext.inl>

#endif // NAZARA_OPENGLRENDERER_WRAPPER_WEB_WEBCONTEXT_HPP
