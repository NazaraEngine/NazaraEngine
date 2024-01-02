// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_MATERIALINSTANCE_HPP
#define NAZARA_GRAPHICS_MATERIALINSTANCE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/MaterialSettings.hpp>
#include <Nazara/Graphics/TransferInterface.hpp>
#include <Nazara/Renderer/RenderBufferView.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <NZSL/Ast/ConstantValue.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

namespace Nz
{
	struct NAZARA_GRAPHICS_API MaterialInstanceParams : ResourceParameters
	{
		MaterialType materialType = MaterialType::Basic;

		bool IsValid() const;
	};

	class Material;
	class MaterialInstance;
	class MaterialPipeline;
	class Texture;

	using MaterialInstanceLibrary = ObjectLibrary<MaterialInstance>;
	using MaterialInstanceLoader = ResourceLoader<MaterialInstance, MaterialInstanceParams>;
	using MaterialInstanceManager = ResourceManager<MaterialInstance, MaterialInstanceParams>;
	using MaterialInstanceSaver = ResourceSaver<MaterialInstance, MaterialInstanceParams>;

	class NAZARA_GRAPHICS_API MaterialInstance : public Resource, public TransferInterface
	{
		struct CopyToken {};

		public:
			using Params = MaterialInstanceParams;

			MaterialInstance(std::shared_ptr<const Material> parent);
			MaterialInstance(const MaterialInstance&) = delete;
			MaterialInstance(const MaterialInstance& material, CopyToken);
			MaterialInstance(MaterialInstance&&) = delete;
			~MaterialInstance();

			inline std::shared_ptr<MaterialInstance> Clone() const;

			void DisablePass(std::string_view passName);
			inline void DisablePass(std::size_t passIndex);
			void EnablePass(std::string_view passName, bool enable = true);
			inline void EnablePass(std::size_t passIndex, bool enable = true);

			void FillShaderBinding(std::vector<ShaderBinding::Binding>& bindings) const;

			inline std::size_t FindTextureProperty(std::string_view propertyName) const;
			inline std::size_t FindValueProperty(std::string_view propertyName) const;

			inline const std::shared_ptr<const Material>& GetParentMaterial() const;
			inline MaterialPassFlags GetPassFlags(std::size_t passIndex) const;
			const std::shared_ptr<MaterialPipeline>& GetPipeline(std::size_t passIndex) const;

			inline const std::shared_ptr<Texture>* GetTextureProperty(std::string_view propertyName) const;
			inline const std::shared_ptr<Texture>& GetTextureProperty(std::size_t textureIndex) const;
			inline const std::shared_ptr<Texture>& GetTexturePropertyOverride(std::size_t textureIndex) const;

			inline const TextureSamplerInfo* GetTextureSamplerProperty(std::string_view propertyName) const;
			inline const TextureSamplerInfo& GetTextureSamplerProperty(std::size_t textureIndex) const;

			inline const MaterialSettings::Value* GetValueProperty(std::string_view propertyName) const;
			inline const MaterialSettings::Value& GetValueProperty(std::size_t valueIndex) const;
			inline const MaterialSettings::Value& GetValuePropertyOverride(std::size_t valueIndex) const;

			bool HasPass(std::string_view passName) const;
			inline bool HasPass(std::size_t passIndex) const;

			void OnTransfer(RenderResources& renderResources, CommandBufferBuilder& builder) override;

			inline void SetTextureProperty(std::string_view propertyName, std::shared_ptr<Texture> texture);
			inline void SetTextureProperty(std::string_view propertyName, std::shared_ptr<Texture> texture, const TextureSamplerInfo& samplerInfo);
			void SetTextureProperty(std::size_t textureIndex, std::shared_ptr<Texture> texture);
			void SetTextureProperty(std::size_t textureIndex, std::shared_ptr<Texture> texture, const TextureSamplerInfo& samplerInfo);

			inline void SetTextureSamplerProperty(std::string_view propertyName, const TextureSamplerInfo& samplerInfo);
			void SetTextureSamplerProperty(std::size_t textureIndex, const TextureSamplerInfo& samplerInfo);

			inline void SetValueProperty(std::string_view propertyName, const MaterialSettings::Value& value);
			void SetValueProperty(std::size_t valueIndex, const MaterialSettings::Value& value);

			void UpdateOptionValue(UInt32 optionHash, const nzsl::Ast::ConstantSingleValue& value);

			void UpdatePassFlags(std::string_view passName, MaterialPassFlags materialFlags);
			inline void UpdatePassFlags(std::size_t passName, MaterialPassFlags materialFlags);

			void UpdatePassStates(std::string_view passName, FunctionRef<bool(RenderStates&)> stateUpdater);
			template<typename F> void UpdatePassStates(std::size_t passIndex, F&& stateUpdater);

			void UpdatePassesStates(std::initializer_list<std::string_view> passesName, FunctionRef<bool(RenderStates&)> stateUpdater);
			template<typename F> void UpdatePassesStates(std::initializer_list<std::size_t> passesIndex, F&& stateUpdater);
			template<typename F> void UpdatePassesStates(F&& stateUpdater, bool ignoreDisabled = true);

			void UpdateTextureBinding(std::size_t textureBinding, std::shared_ptr<Texture> texture, std::shared_ptr<TextureSampler> textureSampler);
			void UpdateUniformBufferData(std::size_t uniformBufferIndex, std::size_t offset, std::size_t size, const void* data);

			MaterialInstance& operator=(const MaterialInstance&) = delete;
			MaterialInstance& operator=(MaterialInstance&&) = delete;

			static constexpr std::size_t InvalidPropertyIndex = MaterialSettings::InvalidPropertyIndex;

			static std::shared_ptr<MaterialInstance> GetDefault(MaterialType materialType, MaterialInstancePreset preset = MaterialInstancePreset::Default);
			static std::shared_ptr<MaterialInstance> LoadFromFile(const std::filesystem::path& filePath, const MaterialInstanceParams& params = MaterialInstanceParams());
			static std::shared_ptr<MaterialInstance> LoadFromMemory(const void* data, std::size_t size, const MaterialInstanceParams& params = MaterialInstanceParams());
			static std::shared_ptr<MaterialInstance> LoadFromStream(Stream& stream, const MaterialInstanceParams& params = MaterialInstanceParams());
			static std::shared_ptr<MaterialInstance> Instantiate(MaterialType materialType, MaterialInstancePreset preset = MaterialInstancePreset::Default);

			NazaraSignal(OnMaterialInstancePipelineInvalidated, const MaterialInstance* /*matInstance*/, std::size_t /*passIndex*/);
			NazaraSignal(OnMaterialInstanceShaderBindingInvalidated, const MaterialInstance* /*matInstance*/);

		private:
			inline void InvalidatePassPipeline(std::size_t passIndex);
			inline void InvalidateShaderBinding();

			struct PassShader
			{
				std::shared_ptr<UberShader> shader;

				NazaraSlot(UberShader, OnShaderUpdated, onShaderUpdated);
			};

			struct PassData
			{
				mutable MaterialPipelineInfo pipelineInfo;
				mutable std::shared_ptr<MaterialPipeline> pipeline;
				std::vector<PassShader> shaders;
				MaterialPassFlags flags;
				bool enabled = false;
			};

			struct TextureBinding
			{
				std::shared_ptr<Texture> texture;
				std::shared_ptr<TextureSampler> sampler;
			};

			struct TextureProperty
			{
				std::shared_ptr<Texture> texture;
				TextureSamplerInfo samplerInfo;
			};

			struct UniformBuffer
			{
				std::size_t bufferIndex;
				std::vector<UInt8> values;
				RenderBufferView bufferView;
				bool dataInvalidated = true;
			};

			std::shared_ptr<const Material> m_parent;
			std::unordered_map<UInt32, nzsl::Ast::ConstantSingleValue> m_optionValuesOverride;
			std::vector<MaterialSettings::Value> m_valueOverride;
			std::vector<PassData> m_passes;
			std::vector<TextureBinding> m_textureBinding;
			std::vector<TextureProperty> m_textureOverride;
			std::vector<UniformBuffer> m_uniformBuffers;
			const MaterialSettings& m_materialSettings;
	};
}

#include <Nazara/Graphics/MaterialInstance.inl>

#endif // NAZARA_GRAPHICS_MATERIALINSTANCE_HPP
