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
#include <Nazara/Renderer/Debug.hpp>

#ifdef NAZARA_DEBUG
	#define NazaraRendererPattern "Nazara?*Renderer-d" NAZARA_DYNLIB_EXTENSION
#else
	#define NazaraRendererPattern "Nazara?*Renderer" NAZARA_DYNLIB_EXTENSION
#endif

namespace Nz
{
	bool Renderer::Initialize()
	{
		if (s_moduleReferenceCounter > 0)
		{
			s_moduleReferenceCounter++;
			return true; // Already initialized
		}

		// Initialize module dependencies
		if (!Utility::Initialize())
		{
			NazaraError("Failed to initialize Utility module");
			return false;
		}

		if (!Platform::Initialize())
		{
			NazaraError("Failed to initialize Platform module");
			return false;
		}

		s_moduleReferenceCounter++;

		CallOnExit onExit(Renderer::Uninitialize);

		NazaraDebug("Searching for renderer implementation");

		DynLib chosenLib;
		std::unique_ptr<RendererImpl> chosenImpl;

		for (auto&& entry : std::filesystem::directory_iterator("."))
		{
			if (!entry.is_regular_file())
				continue;

			const std::filesystem::path& entryPath = entry.path();
			std::filesystem::path fileName = entryPath.filename();
			std::string fileNameStr = fileName.generic_u8string();
			if (!MatchPattern(fileNameStr, NazaraRendererPattern))
				continue;

			NazaraDebug("Trying to load " + fileNameStr);

			DynLib implLib;
			if (!implLib.Load(entryPath))
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
			if (!impl || !impl->Prepare(s_initializationParameters))
			{
				NazaraError("Failed to create renderer implementation");
				continue;
			}

			NazaraDebug("Loaded " + impl->QueryAPIString());

			if (!chosenImpl || impl->IsBetterThan(chosenImpl.get()))
			{
				if (chosenImpl)
					NazaraDebug("Choose " + impl->QueryAPIString() + " over " + chosenImpl->QueryAPIString());

				chosenImpl = std::move(impl); //< Move (and delete previous) implementation before unloading library
				chosenLib = std::move(implLib);
			}
		}

		if (!chosenImpl)
		{
			NazaraError("No renderer found");
			return false;
		}

		s_rendererImpl = std::move(chosenImpl);
		s_rendererLib = std::move(chosenLib);

		NazaraDebug("Using " + s_rendererImpl->QueryAPIString() + " as renderer");

		Buffer::SetBufferFactory(DataStorage_Hardware, CreateHardwareBufferImpl);

		onExit.Reset();

		NazaraNotice("Initialized: Renderer module");
		return true;
	}

	void Renderer::Uninitialize()
	{
		if (s_moduleReferenceCounter != 1)
		{
			// Either the module is not initialized, either it was initialized multiple times
			if (s_moduleReferenceCounter > 1)
				s_moduleReferenceCounter--;

			return;
		}

		s_moduleReferenceCounter = 0;

		// Uninitialize module here
		Buffer::SetBufferFactory(DataStorage_Hardware, nullptr);

		s_rendererImpl.reset();
		s_rendererLib.Unload();

		NazaraNotice("Uninitialized: Renderer module");

		// Free module dependencies
		Platform::Uninitialize();
		Utility::Uninitialize();
	}

	AbstractBuffer* Renderer::CreateHardwareBufferImpl(Buffer* parent, BufferType type)
	{
		return new RenderBuffer(parent, type);
	}

	std::unique_ptr<RendererImpl> Renderer::s_rendererImpl;
	DynLib Renderer::s_rendererLib;
	ParameterList Renderer::s_initializationParameters;
	unsigned int Renderer::s_moduleReferenceCounter = 0;
}
