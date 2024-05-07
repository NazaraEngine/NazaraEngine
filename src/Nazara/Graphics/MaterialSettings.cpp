// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/MaterialSettings.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/TextureAsset.hpp>

namespace Nz
{
	void MaterialSettings::AddPass(std::string_view passName, MaterialPass materialPass)
	{
		std::size_t passIndex = Graphics::Instance()->GetMaterialPassRegistry().GetPassIndex(passName);
		return AddPass(passIndex, std::move(materialPass));
	}

	void MaterialSettings::AddTextureProperty(std::string propertyName, ImageType propertyType, std::shared_ptr<TextureAsset> defaultTexture)
	{
		if (defaultTexture && defaultTexture->GetType() != propertyType)
			throw std::runtime_error("default texture type doesn't match property image type");

		auto& textureProperty = m_textureProperties.emplace_back();
		textureProperty.name = std::move(propertyName);
		textureProperty.type = propertyType;
		textureProperty.defaultTexture = std::move(defaultTexture);
	}

	void MaterialSettings::AddTextureProperty(std::string propertyName, ImageType propertyType, std::shared_ptr<TextureAsset> defaultTexture, const TextureSamplerInfo& defaultSamplerInfo)
	{
		if (defaultTexture && defaultTexture->GetType() != propertyType)
			throw std::runtime_error("default texture type doesn't match property image type");

		auto& textureProperty = m_textureProperties.emplace_back();
		textureProperty.name = std::move(propertyName);
		textureProperty.type = propertyType;
		textureProperty.defaultTexture = std::move(defaultTexture);
		textureProperty.defaultSamplerInfo = defaultSamplerInfo;
	}

	const MaterialPass* MaterialSettings::GetPass(std::string_view passName) const
	{
		std::size_t passIndex = Graphics::Instance()->GetMaterialPassRegistry().GetPassIndex(passName);
		return GetPass(passIndex);
	}
}
