// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/DeferredRenderTechnique.hpp>
#include <Nazara/Graphics/ForwardRenderTechnique.hpp>
#include <Nazara/Graphics/GuillotineTextureAtlas.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/ParticleController.hpp>
#include <Nazara/Graphics/ParticleDeclaration.hpp>
#include <Nazara/Graphics/ParticleGenerator.hpp>
#include <Nazara/Graphics/ParticleRenderer.hpp>
#include <Nazara/Graphics/RenderTechniques.hpp>
#include <Nazara/Graphics/SkinningManager.hpp>
#include <Nazara/Graphics/Loaders/Mesh.hpp>
#include <Nazara/Graphics/Loaders/OBJ.hpp>
#include <Nazara/Graphics/Loaders/Texture.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Utility/Font.hpp>
#include <Nazara/Graphics/Debug.hpp>

bool NzGraphics::Initialize()
{
	if (s_moduleReferenceCounter > 0)
	{
		s_moduleReferenceCounter++;
		return true; // Déjà initialisé
	}

	// Initialisation des dépendances
	if (!NzRenderer::Initialize())
	{
		NazaraError("Failed to initialize Renderer module");
		return false;
	}

	s_moduleReferenceCounter++;

	// Initialisation du module
	NzCallOnExit onExit(NzGraphics::Uninitialize);

	if (!NzMaterial::Initialize())
	{
		NazaraError("Failed to initialize materials");
		return false;
	}

	if (!NzParticleController::Initialize())
	{
		NazaraError("Failed to initialize particle controllers");
		return false;
	}

	if (!NzParticleDeclaration::Initialize())
	{
		NazaraError("Failed to initialize particle declarations");
		return false;
	}

	if (!NzParticleGenerator::Initialize())
	{
		NazaraError("Failed to initialize particle generators");
		return false;
	}

	if (!NzParticleRenderer::Initialize())
	{
		NazaraError("Failed to initialize particle renderers");
		return false;
	}

	if (!NzSkinningManager::Initialize())
	{
		NazaraError("Failed to initialize skinning manager");
		return false;
	}

	// Loaders
	NzLoaders_OBJ_Register();

	// Loaders génériques
	NzLoaders_Mesh_Register();
	NzLoaders_Texture_Register();

	// RenderTechniques
	if (!NzForwardRenderTechnique::Initialize())
	{
		NazaraError("Failed to initialize Forward Rendering");
		return false;
	}

	NzRenderTechniques::Register(NzRenderTechniques::ToString(nzRenderTechniqueType_BasicForward), 0, []() -> NzAbstractRenderTechnique* { return new NzForwardRenderTechnique; });

	if (NzDeferredRenderTechnique::IsSupported())
	{
		if (NzDeferredRenderTechnique::Initialize())
			NzRenderTechniques::Register(NzRenderTechniques::ToString(nzRenderTechniqueType_DeferredShading), 20, []() -> NzAbstractRenderTechnique* { return new NzDeferredRenderTechnique; });
		else
		{
			NazaraWarning("Failed to initialize Deferred Rendering");
		}
	}

	NzFont::SetDefaultAtlas(std::make_shared<NzGuillotineTextureAtlas>());

	onExit.Reset();

	NazaraNotice("Initialized: Graphics module");
	return true;
}

bool NzGraphics::IsInitialized()
{
	return s_moduleReferenceCounter != 0;
}

void NzGraphics::Uninitialize()
{
	if (s_moduleReferenceCounter != 1)
	{
		// Le module est soit encore utilisé, soit pas initialisé
		if (s_moduleReferenceCounter > 1)
			s_moduleReferenceCounter--;

		return;
	}

	// Libération du module
	s_moduleReferenceCounter = 0;

	// Libération de l'atlas s'il vient de nous
	std::shared_ptr<NzAbstractAtlas> defaultAtlas = NzFont::GetDefaultAtlas();
	if (defaultAtlas && defaultAtlas->GetStorage() & nzDataStorage_Hardware)
	{
		NzFont::SetDefaultAtlas(nullptr);

		// La police par défaut peut faire vivre un atlas hardware après la libération du module (ce qui va être problématique)
		// du coup, si la police par défaut utilise un atlas hardware, on lui enlève.
		// Je n'aime pas cette solution mais je n'en ai pas de meilleure sous la main pour l'instant
		if (!defaultAtlas.unique())
		{
			// Encore au moins une police utilise l'atlas
			NzFont* defaultFont = NzFont::GetDefault();
			defaultFont->SetAtlas(nullptr);

			if (!defaultAtlas.unique())
			{
				// Toujours pas seuls propriétaires ? Ah ben zut.
				NazaraWarning("Default font atlas uses hardware storage and is still used");
			}
		}
	}

	defaultAtlas.reset();

	// Loaders
	NzLoaders_Mesh_Unregister();
	NzLoaders_OBJ_Unregister();
	NzLoaders_Texture_Unregister();

	NzDeferredRenderTechnique::Uninitialize();
	NzForwardRenderTechnique::Uninitialize();
	NzSkinningManager::Uninitialize();
	NzParticleRenderer::Uninitialize();
	NzParticleGenerator::Uninitialize();
	NzParticleDeclaration::Uninitialize();
	NzParticleController::Uninitialize();
	NzMaterial::Uninitialize();

	NazaraNotice("Uninitialized: Graphics module");

	// Libération des dépendances
	NzRenderer::Uninitialize();
}

unsigned int NzGraphics::s_moduleReferenceCounter = 0;
