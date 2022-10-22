// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline std::size_t MaterialInstance::FindTextureProperty(std::string_view propertyName)
	{
		return m_materialSettings.FindTextureProperty(propertyName);
	}

	inline std::size_t MaterialInstance::FindValueProperty(std::string_view propertyName)
	{
		return m_materialSettings.FindValueProperty(propertyName);
	}

	inline const std::shared_ptr<Texture>& MaterialInstance::GetTexture(std::size_t textureIndex) const
	{
		if (const std::shared_ptr<Texture>& textureOverride = GetTextureOverride(textureIndex))
			return textureOverride;
		else
			return m_materialSettings.GetTextureProperty(textureIndex).defaultTexture;
	}

	inline const std::shared_ptr<Texture>& MaterialInstance::GetTextureOverride(std::size_t textureIndex) const
	{
		assert(textureIndex < m_textureOverride.size());
		return m_textureOverride[textureIndex].texture;
	}

	inline const MaterialSettings::Value& MaterialInstance::GetValue(std::size_t valueIndex) const
	{
		if (const MaterialSettings::Value& valueOverride = GetValueOverride(valueIndex); !std::holds_alternative<MaterialPropertyNoValue>(valueOverride))
			return valueOverride;
		else
			return m_materialSettings.GetValueProperty(valueIndex).defaultValue;
	}

	inline const MaterialSettings::Value& MaterialInstance::GetValueOverride(std::size_t valueIndex) const
	{
		assert(valueIndex < m_valueOverride.size());
		return m_valueOverride[valueIndex];
	}

	inline void MaterialInstance::SetTexture(std::size_t textureIndex, std::shared_ptr<Texture> texture)
	{
		assert(textureIndex < m_textureOverride.size());
		m_textureOverride[textureIndex].texture = std::move(texture);
	}

	inline void MaterialInstance::SetValue(std::size_t valueIndex, const MaterialSettings::Value& value)
	{
		assert(valueIndex < m_valueOverride.size());
		m_valueOverride[valueIndex] = value;
	}

	inline void MaterialInstance::InvalidateShaderBinding()
	{
		OnMaterialInstanceShaderBindingInvalidated(this);
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
