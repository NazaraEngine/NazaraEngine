// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Web/WebLoader.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Web/WebContext.hpp>


namespace Nz::GL
{
	struct WebLoader::SymbolLoader
	{
		SymbolLoader(WebLoader& parent) :
		loader(parent)
		{
		}

		template<typename FuncType, typename Func>
		bool Load(Func& func, const char* funcName, bool mandatory, bool implementFallback = true)
		{
			func = LoadRaw<FuncType>(funcName);
			if (!func)
			{
				if (!implementFallback || (!loader.ImplementFallback(funcName) && !func)) //< double-check
				{
					if (mandatory)
						throw std::runtime_error("failed to load core function " + std::string(funcName));
				}
			}

			return func != nullptr;
		}

		template<typename FuncType>
		FuncType LoadRaw(const char* funcName)
		{
			return reinterpret_cast<FuncType>(loader.LoadFunction(funcName));
		}

		WebLoader& loader;
	};

	std::shared_ptr<Context> WebLoader::CreateContext(const OpenGLDevice* device, const ContextParams& params, Context* shareContext) const
	{
		std::shared_ptr<WebContext> context = std::make_shared<WebContext>(device, *this);

		if (!context->Create(params, static_cast<WebContext*>(shareContext)))
		{
			NazaraError("failed to create context");
			return {};
		}

		if (!context->Initialize(params))
		{
			NazaraError("failed to initialize context");
			return {};
		}

		return context;
	}

	std::shared_ptr<Context> WebLoader::CreateContext(const OpenGLDevice* device, const ContextParams& params, WindowHandle handle, Context* shareContext) const
	{
		if (handle.type != WindowBackend::Web)
		{
			NazaraError("unsupported window type");
			return {};
		}

		std::shared_ptr<WebContext> context = std::make_shared<WebContext>(device, *this);
		if (!context->Create(params, handle, static_cast<WebContext*>(shareContext)))
		{
			NazaraError("failed to create context");
			return {};
		}

		if (!context->Initialize(params))
		{
			NazaraError("failed to initialize context");
			return {};
		}

		return context;
	}

	ContextType WebLoader::GetPreferredContextType() const
	{
		return ContextType::OpenGL_ES;
	}

	GLFunction WebLoader::LoadFunction(const char* name) const
	{
		return reinterpret_cast<GLFunction>(emscripten_webgl_get_proc_address(name));
	}

	const char* WebLoader::TranslateError(EMSCRIPTEN_RESULT errorId)
	{
		switch (errorId)
		{
			case EMSCRIPTEN_RESULT_SUCCESS: return "The last function succeeded without error.";
			case EMSCRIPTEN_RESULT_DEFERRED: return "The requested operation cannot be completed now for web security reasons, and has been deferred for completion in the next event handler.";
			case EMSCRIPTEN_RESULT_NOT_SUPPORTED: return "The given operation is not supported by this browser or the target element. This value will be returned at the time the callback is registered if the operation is not supported.";
			case EMSCRIPTEN_RESULT_FAILED_NOT_DEFERRED: return "The requested operation could not be completed now for web security reasons. It failed because the user requested the operation not be deferred.";
			case EMSCRIPTEN_RESULT_INVALID_TARGET: return "The operation failed because the specified target element is invalid.";
			case EMSCRIPTEN_RESULT_UNKNOWN_TARGET: return "The operation failed because the specified target element was not found.";
			case EMSCRIPTEN_RESULT_INVALID_PARAM: return "The operation failed because an invalid parameter was passed to the function.";
			case EMSCRIPTEN_RESULT_FAILED: return "Unknown error (EMSCRIPTEN_RESULT_FAILED).";
			case EMSCRIPTEN_RESULT_NO_DATA: return "The operation failed because no data is currently available.";
			default: return "Invalid or unknown error.";
		}
	}

	bool WebLoader::ImplementFallback(std::string_view /*function*/)
	{
		return false;
	}
}

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Core/AntiWindows.hpp>
#endif
