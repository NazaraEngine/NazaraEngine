// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_MATERIALINSTANCE_HPP
#define NAZARA_GRAPHICS_MATERIALINSTANCE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/MaterialSettings.hpp>
#include <Nazara/Graphics/TransferInterface.hpp>
#include <Nazara/Renderer/RenderBufferView.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>
#include <NZSL/Ast/ConstantValue.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

namespace Nz
{
	class Material;
	class MaterialPipeline;
	class Texture;

	class NAZARA_GRAPHICS_API MaterialInstance : public TransferInterface
	{
		public:
			MaterialInstance(std::shared_ptr<Material> parent);
			MaterialInstance(const MaterialInstance&) = delete;
			MaterialInstance(MaterialInstance&&) = delete;
			~MaterialInstance();

			void FillShaderBinding(std::vector<ShaderBinding::Binding>& bindings) const;

			inline std::size_t FindTextureProperty(std::string_view propertyName);
			inline std::size_t FindValueProperty(std::string_view propertyName);

			const std::shared_ptr<MaterialPipeline>& GetPipeline(std::size_t passIndex) const;
			inline const std::shared_ptr<Material>& GetParentMaterial() const;

			inline const std::shared_ptr<Texture>& GetTextureProperty(std::size_t textureIndex) const;
			inline const std::shared_ptr<Texture>& GetTexturePropertyOverride(std::size_t textureIndex) const;

			inline const MaterialSettings::Value& GetValueProperty(std::size_t valueIndex) const;
			inline const MaterialSettings::Value& GetValuePropertyOverride(std::size_t valueIndex) const;

			inline bool HasPass(std::size_t passIndex) const;

			void SetTextureProperty(std::size_t textureIndex, std::shared_ptr<Texture> texture);
			void SetValueProperty(std::size_t valueIndex, const MaterialSettings::Value& value);

			void UpdateOptionValue(UInt32 optionHash, const nzsl::Ast::ConstantSingleValue& value);
			void UpdateTextureBinding(std::size_t textureBinding, std::shared_ptr<Texture> texture);
			void UpdateUniformBufferData(std::size_t uniformBufferIndex, std::size_t offset, std::size_t size, const void* data);

			MaterialInstance& operator=(const MaterialInstance&) = delete;
			MaterialInstance& operator=(MaterialInstance&&) = delete;

			static constexpr std::size_t InvalidPropertyIndex = MaterialSettings::InvalidPropertyIndex;

			NazaraSignal(OnMaterialInstancePipelineInvalidated, const MaterialInstance* /*matInstance*/, std::size_t /*passIndex*/);
			NazaraSignal(OnMaterialInstanceShaderBindingInvalidated, const MaterialInstance* /*matInstance*/);

		private:
			inline void InvalidatePassPipeline(std::size_t passIndex);
			inline void InvalidateShaderBinding();
			void OnTransfer(RenderFrame& renderFrame, CommandBufferBuilder& builder) override;

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
				bool enabled = false;
			};

			struct TextureData
			{
				std::shared_ptr<Texture> texture;
				// TODO: Add sampler override
			};

			struct UniformBuffer
			{
				std::size_t bufferIndex;
				std::vector<UInt8> values;
				RenderBufferView bufferView;
				bool dataInvalidated = true;
			};

			std::shared_ptr<Material> m_parent;
			std::unordered_map<UInt32, nzsl::Ast::ConstantSingleValue> m_optionValuesOverride;
			std::vector<MaterialSettings::Value> m_valueOverride;
			std::vector<PassData> m_passes;
			std::vector<TextureData> m_textureBinding;
			std::vector<TextureData> m_textureOverride;
			std::vector<UniformBuffer> m_uniformBuffers;
			const MaterialSettings& m_materialSettings;
	};
}

#include <Nazara/Graphics/MaterialInstance.inl>

#endif // NAZARA_GRAPHICS_MATERIALINSTANCE_HPP
