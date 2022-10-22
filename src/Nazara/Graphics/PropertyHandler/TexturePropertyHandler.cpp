// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/PropertyHandler/TexturePropertyHandler.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	void TexturePropertyHandler::Setup(const Material& material, const ShaderReflection& reflection)
	{
		const MaterialSettings& settings = material.GetSettings();

		m_propertyIndex = settings.FindTextureProperty(m_propertyName);
		if (m_propertyIndex == MaterialSettings::InvalidPropertyIndex)
			return;

		const auto& textureProperty = settings.GetTextureProperty(m_propertyIndex);

		m_textureIndex = material.FindTextureByTag(m_samplerTag);
		if (m_textureIndex == MaterialSettings::InvalidPropertyIndex)
			return;

		const auto& textureData = material.GetTextureData(m_textureIndex);
		if (textureProperty.type != textureData.imageType)
		{
			// TODO: Use EnumToString to show image type as string
			NazaraError("unmatching texture type: material property is of type " +
				std::to_string(UnderlyingCast(textureProperty.type)) +
				" but shader sampler is of type " +
				std::to_string(UnderlyingCast(textureData.imageType)));
		}
	}
}
