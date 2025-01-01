// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_WEB_WEBCONTEXT_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_WEB_WEBCONTEXT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/OpenGLRenderer/Export.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/Platform/WindowHandle.hpp>
#include <NazaraUtils/StringHash.hpp>
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

			PresentModeFlags GetSupportedPresentModes() const override;

			inline bool HasPlatformExtension(std::string_view str) const;

			void SetPresentMode(PresentMode presentMode) override;

			void SwapBuffers() override;

			WebContext& operator=(const WebContext&) = delete;
			WebContext& operator=(WebContext&&) = delete;

		protected:
			bool ChooseConfig(EmscriptenWebGLContextAttributes* configs, std::size_t maxConfigCount, std::size_t* configCount);
			bool CreateInternal(EmscriptenWebGLContextAttributes config, const WebContext* shareContext = nullptr);

			const WebLoader& m_loader;

		private:
			bool ImplementFallback(std::string_view function) override;

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

			std::unordered_set<std::string, StringHash<>, std::equal_to<>> m_supportedPlatformExtensions;
			EMSCRIPTEN_WEBGL_CONTEXT_HANDLE m_handle;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Web/WebContext.inl>

#endif // NAZARA_OPENGLRENDERER_WRAPPER_WEB_WEBCONTEXT_HPP
