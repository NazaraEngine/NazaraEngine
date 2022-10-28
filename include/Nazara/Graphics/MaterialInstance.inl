// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline std::size_t MaterialInstance::FindTextureProperty(std::string_view propertyName) const
	{
		return m_materialSettings.FindTextureProperty(propertyName);
	}

	inline std::size_t MaterialInstance::FindValueProperty(std::string_view propertyName) const
	{
		return m_materialSettings.FindValueProperty(propertyName);
	}

	inline const std::shared_ptr<Material>& MaterialInstance::GetParentMaterial() const
	{
		return m_parent;
	}

	inline const std::shared_ptr<Texture>& MaterialInstance::GetTextureProperty(std::size_t textureIndex) const
	{
		if (const std::shared_ptr<Texture>& textureOverride = GetTexturePropertyOverride(textureIndex))
			return textureOverride;
		else
			return m_materialSettings.GetTextureProperty(textureIndex).defaultTexture;
	}

	inline const std::shared_ptr<Texture>& MaterialInstance::GetTexturePropertyOverride(std::size_t textureIndex) const
	{
		assert(textureIndex < m_textureOverride.size());
		return m_textureOverride[textureIndex].texture;
	}

	inline const MaterialSettings::Value& MaterialInstance::GetValueProperty(std::size_t valueIndex) const
	{
		if (const MaterialSettings::Value& valueOverride = GetValuePropertyOverride(valueIndex); !std::holds_alternative<MaterialPropertyNoValue>(valueOverride))
			return valueOverride;
		else
			return m_materialSettings.GetValueProperty(valueIndex).defaultValue;
	}

	inline const MaterialSettings::Value& MaterialInstance::GetValuePropertyOverride(std::size_t valueIndex) const
	{
		assert(valueIndex < m_valueOverride.size());
		return m_valueOverride[valueIndex];
	}

	inline bool MaterialInstance::HasPass(std::size_t passIndex) const
	{
		return passIndex < m_passes.size() && m_passes[passIndex].enabled;
	}

	inline void MaterialInstance::InvalidatePassPipeline(std::size_t passIndex)
	{
		assert(passIndex < m_passes.size());
		m_passes[passIndex].pipeline.reset();
		OnMaterialInstancePipelineInvalidated(this, passIndex);
	}

	inline void MaterialInstance::InvalidateShaderBinding()
	{
		OnMaterialInstanceShaderBindingInvalidated(this);
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
