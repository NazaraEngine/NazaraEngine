// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/DeferredRenderTechnique.hpp>
#include <Nazara/Graphics/DepthRenderTechnique.hpp>
#include <Nazara/Graphics/ForwardRenderTechnique.hpp>
#include <Nazara/Graphics/GuillotineTextureAtlas.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/ParticleController.hpp>
#include <Nazara/Graphics/ParticleDeclaration.hpp>
#include <Nazara/Graphics/ParticleGenerator.hpp>
#include <Nazara/Graphics/ParticleRenderer.hpp>
#include <Nazara/Graphics/RenderTechniques.hpp>
#include <Nazara/Graphics/SkinningManager.hpp>
#include <Nazara/Graphics/SkyboxBackground.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Graphics/Formats/MeshLoader.hpp>
#include <Nazara/Graphics/Formats/TextureLoader.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Utility/Font.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	bool Graphics::Initialize()
	{
		if (s_moduleReferenceCounter > 0)
		{
			s_moduleReferenceCounter++;
			return true; // Déjà initialisé
		}

		// Initialisation des dépendances
		if (!Renderer::Initialize())
		{
			NazaraError("Failed to initialize Renderer module");
			return false;
		}

		s_moduleReferenceCounter++;

		// Initialisation du module
		CallOnExit onExit(Graphics::Uninitialize);

		if (!Material::Initialize())
		{
			NazaraError("Failed to initialize materials");
			return false;
		}

		if (!ParticleController::Initialize())
		{
			NazaraError("Failed to initialize particle controllers");
			return false;
		}

		if (!ParticleDeclaration::Initialize())
		{
			NazaraError("Failed to initialize particle declarations");
			return false;
		}

		if (!ParticleGenerator::Initialize())
		{
			NazaraError("Failed to initialize particle generators");
			return false;
		}

		if (!ParticleRenderer::Initialize())
		{
			NazaraError("Failed to initialize particle renderers");
			return false;
		}

		if (!SkinningManager::Initialize())
		{
			NazaraError("Failed to initialize skinning manager");
			return false;
		}

		if (!SkyboxBackground::Initialize())
		{
			NazaraError("Failed to initialize skybox backgrounds");
			return false;
		}

		if (!Sprite::Initialize())
		{
			NazaraError("Failed to initialize sprites");
			return false;
		}

		// Loaders génériques
		Loaders::RegisterMesh();
		Loaders::RegisterTexture();

		// RenderTechniques
		if (!DepthRenderTechnique::Initialize())
		{
			NazaraError("Failed to initialize Depth Rendering");
			return false;
		}

		if (!ForwardRenderTechnique::Initialize())
		{
			NazaraError("Failed to initialize Forward Rendering");
			return false;
		}

		RenderTechniques::Register(RenderTechniques::ToString(RenderTechniqueType_BasicForward), 0, []() -> AbstractRenderTechnique* { return new ForwardRenderTechnique; });

		if (DeferredRenderTechnique::IsSupported())
		{
			if (DeferredRenderTechnique::Initialize())
				RenderTechniques::Register(RenderTechniques::ToString(RenderTechniqueType_DeferredShading), 20, []() -> AbstractRenderTechnique* { return new DeferredRenderTechnique; });
			else
			{
				NazaraWarning("Failed to initialize Deferred Rendering");
			}
		}

		Font::SetDefaultAtlas(std::make_shared<GuillotineTextureAtlas>());

		onExit.Reset();

		NazaraNotice("Initialized: Graphics module");
		return true;
	}

	bool Graphics::IsInitialized()
	{
		return s_moduleReferenceCounter != 0;
	}

	void Graphics::Uninitialize()
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
		std::shared_ptr<AbstractAtlas> defaultAtlas = Font::GetDefaultAtlas();
		if (defaultAtlas && defaultAtlas->GetStorage() & DataStorage_Hardware)
		{
			Font::SetDefaultAtlas(nullptr);

			// La police par défaut peut faire vivre un atlas hardware après la libération du module (ce qui va être problématique)
			// du coup, si la police par défaut utilise un atlas hardware, on lui enlève.
			// Je n'aime pas cette solution mais je n'en ai pas de meilleure sous la main pour l'instant
			if (!defaultAtlas.unique())
			{
				// Encore au moins une police utilise l'atlas
				Font* defaultFont = Font::GetDefault();
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
		Loaders::UnregisterMesh();
		Loaders::UnregisterTexture();

		DeferredRenderTechnique::Uninitialize();
		DepthRenderTechnique::Uninitialize();
		ForwardRenderTechnique::Uninitialize();
		SkinningManager::Uninitialize();
		ParticleRenderer::Uninitialize();
		ParticleGenerator::Uninitialize();
		ParticleDeclaration::Uninitialize();
		ParticleController::Uninitialize();
		Material::Uninitialize();
		SkyboxBackground::Uninitialize();
		Sprite::Uninitialize();

		NazaraNotice("Uninitialized: Graphics module");

		// Libération des dépendances
		Renderer::Uninitialize();
	}

	unsigned int Graphics::s_moduleReferenceCounter = 0;
}
