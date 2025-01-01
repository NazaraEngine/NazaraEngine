// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_HPP
#define NAZARA_GRAPHICS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/FramePipelinePassRegistry.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/MaterialPassRegistry.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Graphics/PipelinePassList.hpp>
#include <Nazara/Graphics/TextureSamplerCache.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/Renderer/RenderPassCache.hpp>
#include <Nazara/Renderer/RenderPipelineLayout.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/TextRenderer/TextRenderer.hpp>
#include <NZSL/FilesystemModuleResolver.hpp>
#include <optional>

namespace Nz
{
	class CommandLineParameters;
	class FilesystemAppComponent;
	class RenderBuffer;
	class TextureAsset;

	class NAZARA_GRAPHICS_API Graphics : public ModuleBase<Graphics>
	{
		friend ModuleBase;
		friend Material;
		friend MaterialInstance;

		public:
			using Dependencies = TypeList<Renderer, TextRenderer>;

			struct Config;
			struct DefaultMaterials;
			struct DefaultTextures;

			Graphics(Config config);
			~Graphics();

			inline const std::shared_ptr<RenderPipeline>& GetBlitPipeline(bool transparent) const;
			inline const std::shared_ptr<RenderPipelineLayout>& GetBlitPipelineLayout() const;
			inline const std::shared_ptr<PipelinePassList>& GetDefaultPipelinePasses() const;
			inline const DefaultTextures& GetDefaultTextures() const;
			inline FramePipelinePassRegistry& GetFramePipelinePassRegistry();
			inline const FramePipelinePassRegistry& GetFramePipelinePassRegistry() const;
			inline MaterialPassRegistry& GetMaterialPassRegistry();
			inline const MaterialPassRegistry& GetMaterialPassRegistry() const;
			inline MaterialInstanceLoader& GetMaterialInstanceLoader();
			inline const MaterialInstanceLoader& GetMaterialInstanceLoader() const;
			inline MaterialLoader& GetMaterialLoader();
			inline const MaterialLoader& GetMaterialLoader() const;
			inline ModelLoader& GetModelLoader();
			inline const ModelLoader& GetModelLoader() const;
			inline PipelinePassListLoader& GetPipelinePassListLoader();
			inline const PipelinePassListLoader& GetPipelinePassListLoader() const;
			inline PixelFormat GetPreferredDepthFormat() const;
			inline PixelFormat GetPreferredDepthStencilFormat() const;
			inline const std::shared_ptr<RenderDevice>& GetRenderDevice() const;
			inline const RenderPassCache& GetRenderPassCache() const;
			inline TextureSamplerCache& GetSamplerCache();
			inline std::shared_ptr<nzsl::FilesystemModuleResolver>& GetShaderModuleResolver();
			inline const std::shared_ptr<nzsl::FilesystemModuleResolver>& GetShaderModuleResolver() const;

			struct NAZARA_GRAPHICS_API Config
			{
				void Override(const CommandLineParameters& parameters);

				RenderDeviceFeatures forceDisableFeatures;
				bool useDedicatedRenderDevice = true;
			};

			struct DefaultMaterials
			{
				struct MaterialData
				{
					std::array<std::shared_ptr<MaterialInstance>, MaterialInstancePresetFlags_ValueCount> presetCache;
					std::shared_ptr<Material> material;
				};

				using PresetModifier = std::function<void(MaterialInstance& matInstance)>;

				EnumArray<MaterialInstancePreset, PresetModifier> presetModifier;
				EnumArray<MaterialType, MaterialData> materials;
			};

			struct DefaultTextures
			{
				EnumArray<ImageType, std::shared_ptr<TextureAsset>> depthTextures;
				EnumArray<ImageType, std::shared_ptr<TextureAsset>> whiteTextures;
			};

		private:
			inline DefaultMaterials& GetDefaultMaterials();

			void BuildBlitPipeline();
			void BuildDefaultMaterials();
			void BuildDefaultPipelinePasses();
			void BuildDefaultTextures();
			void RegisterMaterialPasses();
			void RegisterPipelinePasses();
			void RegisterShaderModules();
			void SelectDepthStencilFormats();

			std::optional<RenderPassCache> m_renderPassCache;
			std::optional<TextureSamplerCache> m_samplerCache;
			std::shared_ptr<nzsl::FilesystemModuleResolver> m_shaderModuleResolver;
			std::shared_ptr<PipelinePassList> m_defaultPipelinePasses;
			std::shared_ptr<RenderDevice> m_renderDevice;
			std::shared_ptr<RenderPipeline> m_blitPipeline;
			std::shared_ptr<RenderPipeline> m_blitPipelineTransparent;
			std::shared_ptr<RenderPipelineLayout> m_blitPipelineLayout;
			DefaultMaterials m_defaultMaterials;
			DefaultTextures m_defaultTextures;
			FramePipelinePassRegistry m_pipelinePassRegistry;
			MaterialInstanceLoader m_materialInstanceLoader;
			MaterialLoader m_materialLoader;
			MaterialPassRegistry m_materialPassRegistry;
			ModelLoader m_modelLoader;
			PipelinePassListLoader m_pipelinePassListLoader;
			PixelFormat m_preferredDepthFormat;
			PixelFormat m_preferredDepthStencilFormat;

			static Graphics* s_instance;
	};
}

#include <Nazara/Graphics/Graphics.inl>

#endif // NAZARA_GRAPHICS_HPP
