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
#include <Nazara/Utility/Utility.hpp>
#include <filesystem>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

#ifdef NAZARA_PLATFORM_WINDOWS
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
	Renderer::Renderer() :
	ModuleBase("Renderer", this)
	{
		struct RendererImplementations
		{
			std::filesystem::path fileName;
			int score;
		};
		std::vector<RendererImplementations> implementations;

		auto RegisterImpl = [&](std::filesystem::path fileName, auto ComputeScore)
		{
			fileName.replace_extension(NAZARA_DYNLIB_EXTENSION);

			int score = ComputeScore();
			if (score >= 0)
			{
				auto& impl = implementations.emplace_back();
				impl.fileName = std::move(fileName);
				impl.score = score;
			}
		};

		RegisterImpl(NazaraRendererPrefix "NazaraOpenGLRenderer" NazaraRendererDebugSuffix, [] { return 50; });
		RegisterImpl(NazaraRendererPrefix "NazaraVulkanRenderer" NazaraRendererDebugSuffix, [] { return 100; });

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

		Buffer::SetBufferFactory(DataStorage_Hardware, [](Buffer* parent, BufferType type) -> AbstractBuffer* { return new RenderBuffer(parent, type); });
	}

	Renderer::~Renderer()
	{
		// Uninitialize module here
		Buffer::SetBufferFactory(DataStorage_Hardware, nullptr);

		m_rendererImpl.reset();
	}

	Renderer* Renderer::s_instance = nullptr;
}
