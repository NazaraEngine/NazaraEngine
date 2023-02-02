// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Web/WebLoader.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Web/WebContext.hpp>

#include <Nazara/OpenGLRenderer/Debug.hpp>

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

	std::unique_ptr<Context> WebLoader::CreateContext(const OpenGLDevice* device, const ContextParams& params, Context* shareContext) const
	{
		std::unique_ptr<WebContext> context = std::make_unique<WebContext>(device, *this);

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

	std::unique_ptr<Context> WebLoader::CreateContext(const OpenGLDevice* device, const ContextParams& params, WindowHandle handle, Context* shareContext) const
	{
		std::unique_ptr<WebContext> context;
		switch (handle.type)
		{
			case WindowBackend::Invalid:
				break;

			case WindowBackend::Web:
				context = std::make_unique<WebContext>(device, *this);
				break;
		}

		if (!context)
		{
			NazaraError("unsupported window type");
			return {};
		}

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
		/*GLFunction func = reinterpret_cast<GLFunction>(m_WebLib.GetSymbol(name));
		if (!func && WebGetProcAddress)
			func = reinterpret_cast<GLFunction>(WebGetProcAddress(name));

		return func;*/

		return reinterpret_cast<GLFunction>(emscripten_webgl_get_proc_address(name));

		//return nullptr;
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

	bool WebLoader::ImplementFallback(const std::string_view& /*function*/)
	{
		return false;
	}
}

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Core/AntiWindows.hpp>
#endif
