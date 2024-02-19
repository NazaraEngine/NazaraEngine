// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Core/CommandLineParameters.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/EnvironmentVariables.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Renderer/RendererImpl.hpp>
#include <NazaraUtils/EnumArray.hpp>
#include <NazaraUtils/TypeTraits.hpp>
#include <frozen/string.h>
#include <frozen/unordered_map.h>
#include <filesystem>
#include <stdexcept>

#ifdef NAZARA_RENDERER_EMBEDDEDBACKENDS

#include <Nazara/OpenGLRenderer/OpenGLRenderer.hpp>

#ifndef NAZARA_PLATFORM_WEB
#include <Nazara/VulkanRenderer/VulkanRenderer.hpp>
#endif

#endif


#ifdef NAZARA_COMPILER_MSVC
#define NazaraRendererPrefix ""
#else
#define NazaraRendererPrefix "lib"
#endif

#ifdef NAZARA_DEBUG
	#define NazaraRendererDebugSuffix "-d"
#else
	#define NazaraRendererDebugSuffix ""
#endif

namespace Nz
{
	Renderer::Renderer(Config config) :
	ModuleBase("Renderer", this),
	m_config(std::move(config))
	{
		LoadBackend(m_config);
	}

	Renderer::~Renderer()
	{
		// reset Renderer impl before unloading library
		m_rendererImpl.reset();
	}

	std::shared_ptr<RenderDevice> Renderer::InstanciateRenderDevice(std::size_t deviceIndex, const RenderDeviceFeatures& enabledFeatures)
	{
		return m_rendererImpl->InstanciateRenderDevice(deviceIndex, enabledFeatures);
	}

	RenderAPI Renderer::QueryAPI() const
	{
		return m_rendererImpl->QueryAPI();
	}

	std::string Renderer::QueryAPIString() const
	{
		return m_rendererImpl->QueryAPIString();
	}

	UInt32 Renderer::QueryAPIVersion() const
	{
		return m_rendererImpl->QueryAPIVersion();
	}

	const std::vector<RenderDeviceInfo>& Renderer::QueryRenderDevices() const
	{
		return m_rendererImpl->QueryRenderDevices();
	}

	void Renderer::LoadBackend(const Config& config)
	{
		using FactoryFunc = FunctionPtr<std::unique_ptr<RendererImpl>()>;

		struct RendererImplementations
		{
#ifdef NAZARA_RENDERER_EMBEDDEDBACKENDS
			FactoryFunc factory;
#else
			std::filesystem::path fileName;
#endif
			int score;
		};
		std::vector<RendererImplementations> implementations;

		RenderAPI preferredAPI = config.preferredAPI;
		// OpenGL and OpenGL ES are handled by the same renderer (OpenGL ES is handled in OpenGLRenderer code)
		if (preferredAPI == RenderAPI::OpenGL_ES)
			preferredAPI = RenderAPI::OpenGL;

#ifdef NAZARA_RENDERER_EMBEDDEDBACKENDS
		auto RegisterImpl = [&]<typename T>(RenderAPI api, auto ComputeScore, TypeTag<T>)
		{
			int score = ComputeScore();
			if (score >= 0)
			{
				auto& impl = implementations.emplace_back();
				impl.factory = []() -> std::unique_ptr<RendererImpl> { return std::make_unique<T>(); };
				impl.score = (preferredAPI == api) ? std::numeric_limits<int>::max() : score;
			}
		};

		RegisterImpl(RenderAPI::OpenGL, [] { return 50; }, TypeTag<OpenGLRenderer>{});
#ifndef NAZARA_PLATFORM_WEB
		RegisterImpl(RenderAPI::Vulkan, [] { return 100; }, TypeTag<VulkanRenderer>{});
#endif

#else
		constexpr EnumArray<RenderAPI, const char*> rendererPaths = {
			NazaraRendererPrefix "NazaraDirect3DRenderer" NazaraRendererDebugSuffix, // Direct3D
			NazaraRendererPrefix "NazaraMantleRenderer"   NazaraRendererDebugSuffix, // Mantle
			NazaraRendererPrefix "NazaraMetalRenderer"    NazaraRendererDebugSuffix, // Metal
			NazaraRendererPrefix "NazaraOpenGLRenderer"   NazaraRendererDebugSuffix, // OpenGL
			NazaraRendererPrefix "NazaraOpenGLRenderer"   NazaraRendererDebugSuffix, // OpenGL_ES
			NazaraRendererPrefix "NazaraVulkanRenderer"   NazaraRendererDebugSuffix, // Vulkan

			nullptr // Unknown
		};

		auto RegisterImpl = [&](RenderAPI api, auto ComputeScore)
		{
			const char* rendererName = rendererPaths[api];
			assert(rendererName);

			std::filesystem::path fileName(rendererName);
			fileName.replace_extension(NAZARA_DYNLIB_EXTENSION);

			int score = ComputeScore();
			if (score >= 0)
			{
				auto& impl = implementations.emplace_back();
				impl.fileName = std::move(fileName);
				impl.score = (preferredAPI == api) ? std::numeric_limits<int>::max() : score;
			}
		};

		RegisterImpl(RenderAPI::OpenGL, [] { return 50; });
#ifndef NAZARA_PLATFORM_WEB
		RegisterImpl(RenderAPI::Vulkan, [] { return 100; });
#endif

#endif

		std::sort(implementations.begin(), implementations.end(), [](const auto& lhs, const auto& rhs) { return lhs.score > rhs.score; });

		std::unique_ptr<RendererImpl> chosenImpl;
#ifndef NAZARA_RENDERER_EMBEDDEDBACKENDS
		NazaraDebug("Searching for renderer implementation");

		DynLib chosenLib;
#endif

		for (auto&& rendererImpl : implementations)
		{
#ifndef NAZARA_RENDERER_EMBEDDEDBACKENDS
			DynLib implLib;
			if (!implLib.Load(rendererImpl.fileName))
			{
				NazaraWarningFmt("Failed to load {0}: {1}", rendererImpl.fileName, implLib.GetLastError());
				continue;
			}

			CreateRendererImplFunc createRenderer = reinterpret_cast<CreateRendererImplFunc>(implLib.GetSymbol("NazaraRenderer_Instantiate"));
			if (!createRenderer)
			{
				NazaraDebug("Skipped {0} (NazaraRenderer_Instantiate symbol not found)", rendererImpl.fileName);
				continue;
			}

			std::unique_ptr<RendererImpl> impl(createRenderer());
#else
			std::unique_ptr<RendererImpl> impl = rendererImpl.factory();
#endif
			if (!impl || !impl->Prepare(config))
			{
				NazaraError("failed to create renderer implementation");
				continue;
			}

#ifndef NAZARA_RENDERER_EMBEDDEDBACKENDS
			NazaraDebug("Loaded {0}", rendererImpl.fileName);
			chosenLib = std::move(implLib);
#endif

			chosenImpl = std::move(impl); //< Move (and delete previous) implementation before unloading library
			break;
		}

		if (!chosenImpl)
			throw std::runtime_error("no renderer found");

		m_rendererImpl = std::move(chosenImpl);
#ifndef NAZARA_RENDERER_EMBEDDEDBACKENDS
		m_rendererLib = std::move(chosenLib);
#endif

		NazaraDebug("Using {0} as renderer", m_rendererImpl->QueryAPIString());
	}

	Renderer* Renderer::s_instance = nullptr;


	void Renderer::Config::Override(const CommandLineParameters& parameters)
	{
		std::string_view value;
		auto GetParameter = [&](std::string_view paramName, const char* envName, std::string_view* var)
		{
			if (parameters.GetParameter(paramName, var))
				return true;

			if (const char* envValue = GetEnvironmentVariable(envName); envValue && *envValue != '\0')
			{
				*var = envValue;
				return true;
			}

			return false;
		};

		if (GetParameter("render-api", "NAZARA_RENDER_API", &value))
		{
			constexpr auto renderAPIStr = frozen::make_unordered_map<frozen::string, RenderAPI>({
				{ "auto",     RenderAPI::Unknown },
				{ "direct3d", RenderAPI::Direct3D },
				{ "mantle",   RenderAPI::Mantle },
				{ "metal",    RenderAPI::Metal },
				{ "opengl",   RenderAPI::OpenGL },
				{ "opengles", RenderAPI::OpenGL_ES },
				{ "vulkan",   RenderAPI::Vulkan }
			});

			if (auto it = renderAPIStr.find(value); it != renderAPIStr.end())
				preferredAPI = it->second;
			else
				NazaraErrorFmt("unknown render API \"{0}\"", value);
		}

		if (GetParameter("render-api-validation", "NAZARA_RENDER_API_VALDATION", &value))
		{
			constexpr auto validationStr = frozen::make_unordered_map<frozen::string, RenderAPIValidationLevel>({
				{ "debug",    RenderAPIValidationLevel::Debug },
				{ "errors",   RenderAPIValidationLevel::Errors },
				{ "none",     RenderAPIValidationLevel::None },
				{ "verbose",  RenderAPIValidationLevel::Verbose },
				{ "warnings", RenderAPIValidationLevel::Warnings }
			});

			if (auto it = validationStr.find(value); it != validationStr.end())
				validationLevel = it->second;
			else
				NazaraErrorFmt("unknown validation level \"{0}\"", value);
		}
	}
}
