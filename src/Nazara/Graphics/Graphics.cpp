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
#include <Nazara/Graphics/TileMap.hpp>
#include <Nazara/Graphics/Formats/MeshLoader.hpp>
#include <Nazara/Graphics/Formats/TextureLoader.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Utility/Font.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::Graphics
	* \brief Graphics class that represents the module initializer of Graphics
	*/

	/*!
	* \brief Initializes the Graphics module
	* \return true if initialization is successful
	*
	* \remark Produces a NazaraNotice
	* \remark Produces a NazaraError if one submodule failed
	*/

	bool Graphics::Initialize()
	{
		if (IsInitialized())
		{
			s_moduleReferenceCounter++;
			return true; // Already initialized
		}

		// Initialisation of dependances
		if (!Renderer::Initialize())
		{
			NazaraError("Failed to initialize Renderer module");
			return false;
		}

		s_moduleReferenceCounter++;

		// Initialisation of the module
		CallOnExit onExit(Graphics::Uninitialize);

		// Materials
		if (!MaterialPipeline::Initialize())
		{
			NazaraError("Failed to initialize material pipelines");
			return false;
		}

		if (!Material::Initialize())
		{
			NazaraError("Failed to initialize materials");
			return false;
		}

		// Renderables
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

		if (!TileMap::Initialize())
		{
			NazaraError("Failed to initialize tilemaps");
			return false;
		}

		// Generic loaders
		Loaders::RegisterMesh();
		Loaders::RegisterTexture();

		// Render techniques
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

	/*!
	* \brief Checks whether the module is initialized
	* \return true if module is initialized
	*/

	bool Graphics::IsInitialized()
	{
		return s_moduleReferenceCounter != 0;
	}

	/*!
	* \brief Uninitializes the Core module
	*
	* \remark Produces a NazaraNotice
	*/

	void Graphics::Uninitialize()
	{
		if (s_moduleReferenceCounter != 1)
		{
			// The module is still in use, or can not be uninitialized
			if (s_moduleReferenceCounter > 1)
				s_moduleReferenceCounter--;

			return;
		}

		// Free of module
		s_moduleReferenceCounter = 0;

		// Free of atlas if it is ours
		std::shared_ptr<AbstractAtlas> defaultAtlas = Font::GetDefaultAtlas();
		if (defaultAtlas && defaultAtlas->GetStorage() == DataStorage_Hardware)
		{
			Font::SetDefaultAtlas(nullptr);

			// The default police can make live one hardware atlas after the free of a module (which could be problematic)
			// So, if the default police use a hardware atlas, we stole it.
			// I don't like this solution, but I don't have any better
			if (!defaultAtlas.unique())
			{
				// Still at least one police use the atlas
				Font* defaultFont = Font::GetDefault();
				defaultFont->SetAtlas(nullptr);

				if (!defaultAtlas.unique())
				{
					// Still not the only one to own it ? Then crap.
					NazaraWarning("Default font atlas uses hardware storage and is still used");
				}
			}
		}

		defaultAtlas.reset();

		// Loaders
		Loaders::UnregisterMesh();
		Loaders::UnregisterTexture();

		// Renderables
		ParticleRenderer::Uninitialize();
		ParticleGenerator::Uninitialize();
		ParticleDeclaration::Uninitialize();
		ParticleController::Uninitialize();
		SkyboxBackground::Uninitialize();
		Sprite::Uninitialize();
		TileMap::Uninitialize();

		// Render techniques
		DeferredRenderTechnique::Uninitialize();
		DepthRenderTechnique::Uninitialize();
		ForwardRenderTechnique::Uninitialize();
		SkinningManager::Uninitialize();

		// Materials
		Material::Uninitialize();
		MaterialPipeline::Uninitialize();

		NazaraNotice("Uninitialized: Graphics module");

		// Free of dependances
		Renderer::Uninitialize();
	}

	unsigned int Graphics::s_moduleReferenceCounter = 0;
}
