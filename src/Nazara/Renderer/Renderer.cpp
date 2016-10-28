// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Directory.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <Nazara/Renderer/Debug.hpp>

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

		s_moduleReferenceCounter++;

		CallOnExit onExit(Renderer::Uninitialize);

		NazaraDebug("Searching for renderer implementation");

		Directory dir(".");
		dir.SetPattern("Nazara?*Renderer*" NAZARA_DYNLIB_EXTENSION); //< Ex: NazaraVulkanRenderer.dll

		if (!dir.Open())
		{
			NazaraError("Failed to open directory");
			return false;
		}

		DynLib chosenLib;
		std::unique_ptr<RendererImpl> chosenImpl;
		while (dir.NextResult())
		{
			NazaraDebug("Trying to load " + dir.GetResultName());

			DynLib implLib;
			if (!implLib.Load(dir.GetResultPath()))
			{
				NazaraWarning("Failed to load " + dir.GetResultName() + ": " + implLib.GetLastError());
				continue;
			}

			CreateRendererImplFunc createRenderer = reinterpret_cast<CreateRendererImplFunc>(implLib.GetSymbol("NazaraRenderer_Instantiate"));
			if (!createRenderer)
			{
				NazaraDebug("Skipped " + dir.GetResultName() + " (symbol not found)");
				continue;
			}

			std::unique_ptr<RendererImpl> impl(createRenderer());
			if (!impl || !impl->Prepare(Nz::ParameterList()))
			{
				NazaraError("Failed to create renderer implementation");
				continue;
			}

			NazaraDebug("Loaded " + impl->QueryAPIString());

			if (!chosenImpl || impl->IsBetterThan(chosenImpl.get()))
			{
				if (chosenImpl)
					NazaraDebug("Choose " + impl->QueryAPIString() + " over " + chosenImpl->QueryAPIString());

				chosenLib = std::move(implLib);
				chosenImpl = std::move(impl);
			}
		}

		s_rendererImpl = std::move(chosenImpl);
		s_rendererLib = std::move(chosenLib);

		NazaraDebug("Using " + s_rendererImpl->QueryAPIString() + " as renderer");

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
		s_rendererImpl.reset();
		s_rendererLib.Unload();

		NazaraNotice("Uninitialized: Renderer module");

		// Free module dependencies
		Utility::Uninitialize();
	}

	DynLib Renderer::s_rendererLib;
	std::unique_ptr<RendererImpl> Renderer::s_rendererImpl;
	unsigned int Renderer::s_moduleReferenceCounter = 0;
}
