// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/PropertyHandler/TexturePropertyHandler.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	bool TexturePropertyHandler::NeedsUpdateOnTextureUpdate(std::size_t updatedPropertyIndex) const
	{
		return m_propertyIndex == updatedPropertyIndex;
	}

	void TexturePropertyHandler::Setup(const Material& material, const ShaderReflection& reflection)
	{
		m_propertyIndex = MaterialSettings::InvalidPropertyIndex;

		const MaterialSettings& settings = material.GetSettings();

		std::size_t propertyIndex = settings.FindTextureProperty(m_propertyName);
		if (propertyIndex == MaterialSettings::InvalidPropertyIndex)
			return;

		const auto& textureProperty = settings.GetTextureProperty(propertyIndex);

		m_textureIndex = material.FindTextureByTag(m_samplerTag);
		if (m_textureIndex == Material::InvalidIndex)
			return;

		const auto& textureData = material.GetTextureData(m_textureIndex);
		if (textureProperty.type != textureData.imageType)
		{
			// TODO: Use EnumToString to show image type as string
			NazaraError("unmatching texture type: material property is of type " +
				std::to_string(UnderlyingCast(textureProperty.type)) +
				" but shader sampler is of type " +
				std::to_string(UnderlyingCast(textureData.imageType)));

			return;
		}

		m_propertyIndex = propertyIndex;

		m_optionHash = 0;
		if (const ShaderReflection::OptionData* optionData = reflection.GetOptionByName(m_optionName))
		{
			if (IsPrimitiveType(optionData->type) && std::get<nzsl::Ast::PrimitiveType>(optionData->type) == nzsl::Ast::PrimitiveType::Boolean)
			{
				NazaraAssert(optionData->hash != 0, "unexpected option hash");
				m_optionHash = optionData->hash;
			}
			else
				NazaraError("option " + m_optionName + " is not a boolean option (got " + ToString(optionData->type) + ")");
		}
	}

	void TexturePropertyHandler::Update(MaterialInstance& materialInstance) const
	{
		if (m_propertyIndex == MaterialSettings::InvalidPropertyIndex)
			return;

		const std::shared_ptr<Texture>& texture = materialInstance.GetTextureProperty(m_propertyIndex);
		const std::shared_ptr<TextureSampler>& sampler = Graphics::Instance()->GetSamplerCache().Get(materialInstance.GetTextureSamplerProperty(m_propertyIndex));

		materialInstance.UpdateTextureBinding(m_textureIndex, texture, sampler);
		if (m_optionHash != 0)
			materialInstance.UpdateOptionValue(m_optionHash, texture != nullptr);
	}
}
