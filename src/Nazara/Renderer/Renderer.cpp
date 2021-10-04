// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Platform/Platform.hpp>
#include <Nazara/Renderer/RenderBuffer.hpp>
#include <Nazara/Utility/AbstractBuffer.hpp>
#include <Nazara/Utility/Buffer.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <filesystem>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

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
	ModuleBase("Renderer", this)
	{
		LoadBackend(config);

		Buffer::SetBufferFactory(DataStorage::Hardware, [](Buffer* parent, BufferType type) -> std::unique_ptr<AbstractBuffer> { return std::make_unique<RenderBuffer>(parent, type); });
	}

	Renderer::~Renderer()
	{
		// Uninitialize module here
		Buffer::SetBufferFactory(DataStorage::Hardware, nullptr);

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
			std::filesystem::path fileName;
			int score;
		};
		std::vector<RendererImplementations> implementations;

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

		std::sort(implementations.begin(), implementations.end(), [](const auto& lhs, const auto& rhs) { return lhs.score > rhs.score; });

		NazaraDebug("Searching for renderer implementation");

		DynLib chosenLib;
		std::unique_ptr<RendererImpl> chosenImpl;

		for (auto&& rendererImpl : implementations)
		{
			if (!std::filesystem::exists(rendererImpl.fileName))
				continue;

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
			if (!impl || !impl->Prepare({}))
			{
				NazaraError("Failed to create renderer implementation");
				continue;
			}

			NazaraDebug("Loaded " + fileNameStr);

			chosenImpl = std::move(impl); //< Move (and delete previous) implementation before unloading library
			chosenLib = std::move(implLib);
			break;
		}

		if (!chosenImpl)
			throw std::runtime_error("no renderer found");

		m_rendererImpl = std::move(chosenImpl);
		m_rendererLib = std::move(chosenLib);

		NazaraDebug("Using " + m_rendererImpl->QueryAPIString() + " as renderer");
	}

	Renderer* Renderer::s_instance = nullptr;
}
