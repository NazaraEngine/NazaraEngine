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
#include <memory>
#include <vector>

namespace Nz
{
	class Material;
	class MaterialPass;
	class Texture;

	class NAZARA_GRAPHICS_API MaterialInstance : public TransferInterface
	{
		public:
			MaterialInstance(std::shared_ptr<Material> parent);
			MaterialInstance(const MaterialInstance&) = delete;
			MaterialInstance(MaterialInstance&&) = delete;
			~MaterialInstance();

			inline std::size_t FindTextureProperty(std::string_view propertyName);
			inline std::size_t FindValueProperty(std::string_view propertyName);

			const std::shared_ptr<MaterialPass>& GetPass(std::size_t passIndex) const;

			inline const std::shared_ptr<Texture>& GetTexture(std::size_t textureIndex) const;
			inline const std::shared_ptr<Texture>& GetTextureOverride(std::size_t textureIndex) const;

			inline const MaterialSettings::Value& GetValue(std::size_t valueIndex) const;
			inline const MaterialSettings::Value& GetValueOverride(std::size_t valueIndex) const;

			inline void SetTexture(std::size_t textureIndex, std::shared_ptr<Texture> texture);
			inline void SetValue(std::size_t valueIndex, const MaterialSettings::Value& value);

			MaterialInstance& operator=(const MaterialInstance&) = delete;
			MaterialInstance& operator=(MaterialInstance&&) = delete;

			static constexpr std::size_t InvalidPropertyIndex = MaterialSettings::InvalidPropertyIndex;

			NazaraSignal(OnMaterialInstanceShaderBindingInvalidated, const MaterialInstance* /*matInstance*/);

		private:
			inline void InvalidateShaderBinding();
			void OnTransfer(RenderFrame& renderFrame, CommandBufferBuilder& builder) override;

			struct TextureOverride
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
			std::vector<TextureOverride> m_textureOverride;
			std::vector<MaterialSettings::Value> m_valueOverride;
			std::vector<UniformBuffer> m_uniformBuffers;
			const MaterialSettings& m_materialSettings;
	};
}

#include <Nazara/Graphics/MaterialInstance.inl>

#endif // NAZARA_GRAPHICS_MATERIALINSTANCE_HPP
