// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Platform/Platform.hpp>
#include <Nazara/Renderer/RenderBuffer.hpp>
#include <Nazara/Utility/Buffer.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <Nazara/Utils/CallOnExit.hpp>
#include <filesystem>
#include <stdexcept>

#ifdef NAZARA_RENDERER_EMBEDDEDBACKENDS
#include <Nazara/OpenGLRenderer/OpenGLRenderer.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderer.hpp>
#endif

#include <Nazara/Renderer/Debug.hpp>

#ifdef NAZARA_COMPILER_MSVC
#define NazaraRendererPrefix "./"
#else
#define NazaraRendererPrefix "./lib"
#endif

#ifdef NAZARA_DEBUG
	#define NazaraRendererDebugSuffix "-d"
#else
	#define NazaraRendererDebugSuffix ""
#endif

namespace Nz
{
	Renderer::Renderer(Config config) :
	ModuleBase("Renderer", this)
	{
		LoadBackend(config);
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
		constexpr std::array<const char*, RenderAPICount> rendererPaths = {
			NazaraRendererPrefix "NazaraDirect3DRenderer" NazaraRendererDebugSuffix, // Direct3D
			NazaraRendererPrefix "NazaraMantleRenderer"   NazaraRendererDebugSuffix, // Mantle
			NazaraRendererPrefix "NazaraMetalRenderer"    NazaraRendererDebugSuffix, // Metal
			NazaraRendererPrefix "NazaraOpenGLRenderer"   NazaraRendererDebugSuffix, // OpenGL
			NazaraRendererPrefix "NazaraVulkanRenderer"   NazaraRendererDebugSuffix, // Vulkan

			nullptr // Unknown
		};

		struct RendererImplementations
		{
#ifdef NAZARA_RENDERER_EMBEDDEDBACKENDS
			std::function<std::unique_ptr<RendererImpl>()> factory;
#else
			std::filesystem::path fileName;
#endif
			int score;
		};
		std::vector<RendererImplementations> implementations;

#ifdef NAZARA_RENDERER_EMBEDDEDBACKENDS
		auto RegisterImpl = [&](RenderAPI api, auto ComputeScore, std::function<std::unique_ptr<RendererImpl>()> factory)
		{
			const char* rendererName = rendererPaths[UnderlyingCast(api)];
			assert(rendererName);

			std::filesystem::path fileName(rendererName);
			fileName.replace_extension(NAZARA_DYNLIB_EXTENSION);

			int score = ComputeScore();
			if (score >= 0)
			{
				auto& impl = implementations.emplace_back();
				impl.factory = std::move(factory);
				impl.score = (config.preferredAPI == api) ? std::numeric_limits<int>::max() : score;
			}
		};

		RegisterImpl(RenderAPI::OpenGL, [] { return 50; }, [] { return std::make_unique<OpenGLRenderer>(); });
		RegisterImpl(RenderAPI::Vulkan, [] { return 100; }, [] { return std::make_unique<VulkanRenderer>(); });
#else
		auto RegisterImpl = [&](RenderAPI api, auto ComputeScore)
		{
			const char* rendererName = rendererPaths[UnderlyingCast(api)];
			assert(rendererName);

			std::filesystem::path fileName(rendererName);
			fileName.replace_extension(NAZARA_DYNLIB_EXTENSION);

			int score = ComputeScore();
			if (score >= 0)
			{
				auto& impl = implementations.emplace_back();
				impl.fileName = std::move(fileName);
				impl.score = (config.preferredAPI == api) ? std::numeric_limits<int>::max() : score;
			}
		};

		RegisterImpl(RenderAPI::OpenGL, [] { return 50; });
		RegisterImpl(RenderAPI::Vulkan, [] { return 100; });
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
			std::string fileNameStr = rendererImpl.fileName.generic_u8string();

			DynLib implLib;
			if (!implLib.Load(rendererImpl.fileName))
			{
				NazaraWarning("Failed to load " + fileNameStr + ": " + implLib.GetLastError());
				continue;
			}

			CreateRendererImplFunc createRenderer = reinterpret_cast<CreateRendererImplFunc>(implLib.GetSymbol("NazaraRenderer_Instantiate"));
			if (!createRenderer)
			{
				NazaraDebug("Skipped " + fileNameStr + " (symbol not found)");
				continue;
			}

			std::unique_ptr<RendererImpl> impl(createRenderer());
#else
			std::unique_ptr<RendererImpl> impl = rendererImpl.factory();
#endif
			if (!impl || !impl->Prepare({}))
			{
				NazaraError("Failed to create renderer implementation");
				continue;
			}

#ifndef NAZARA_RENDERER_EMBEDDEDBACKENDS
			NazaraDebug("Loaded " + fileNameStr);
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

		NazaraDebug("Using " + m_rendererImpl->QueryAPIString() + " as renderer");
	}

	Renderer* Renderer::s_instance = nullptr;
}
