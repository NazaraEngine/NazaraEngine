// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/MaterialSettings.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/TextureAsset.hpp>
#include <Nazara/Renderer/RenderBuffer.hpp>

namespace Nz
{
	void MaterialSettings::AddPass(std::string_view passName, MaterialPass materialPass)
	{
		std::size_t passIndex = Graphics::Instance()->GetMaterialPassRegistry().GetPassIndex(passName);
		return AddPass(passIndex, std::move(materialPass));
	}

	void MaterialSettings::AddBufferProperty(std::string propertyName, std::shared_ptr<RenderBuffer> defaultBuffer)
	{
		UInt64 size = (defaultBuffer) ? defaultBuffer->GetSize() : 0;
		return AddBufferProperty(std::move(propertyName), std::move(defaultBuffer), 0, size);
	}

	void MaterialSettings::AddBufferProperty(std::string propertyName, std::shared_ptr<RenderBuffer> defaultBuffer, UInt64 defaultOffset, UInt64 defaultSize)
	{
		NazaraAssert(!defaultBuffer || defaultBuffer->GetType() == Nz::BufferType::Storage, "a default buffer was passed but wasn't a storage buffer");

		auto& storageBufferProperty = m_bufferProperties.emplace_back();
		storageBufferProperty.name = std::move(propertyName);
		storageBufferProperty.defaultValue.buffer = std::move(defaultBuffer);
		storageBufferProperty.defaultValue.offset = defaultOffset;
		storageBufferProperty.defaultValue.size = defaultSize;
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
