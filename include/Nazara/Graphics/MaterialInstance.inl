// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <type_traits>

namespace Nz
{
	inline std::shared_ptr<MaterialInstance> MaterialInstance::Clone() const
	{
		return std::make_shared<MaterialInstance>(*this, CopyToken{});
	}

	inline void MaterialInstance::DisablePass(std::size_t passIndex)
	{
		EnablePass(passIndex, false);
	}

	inline void MaterialInstance::EnablePass(std::size_t passIndex, bool enable)
	{
		assert(passIndex < m_passes.size());
		if (m_passes[passIndex].enabled != enable)
		{
			m_passes[passIndex].enabled = enable;
			InvalidatePassPipeline(passIndex);
		}
	}

	inline std::size_t MaterialInstance::FindBufferProperty(std::string_view propertyName) const
	{
		return m_materialSettings.FindBufferProperty(propertyName);
	}

	inline std::size_t MaterialInstance::FindTextureProperty(std::string_view propertyName) const
	{
		return m_materialSettings.FindTextureProperty(propertyName);
	}

	inline std::size_t MaterialInstance::FindValueProperty(std::string_view propertyName) const
	{
		return m_materialSettings.FindValueProperty(propertyName);
	}

	inline const MaterialSettings::BufferValue* MaterialInstance::GetBufferProperty(std::string_view propertyName) const
	{
		std::size_t propertyIndex = FindBufferProperty(propertyName);
		if (propertyIndex == MaterialSettings::InvalidPropertyIndex)
		{
			NazaraError("material has no storage buffer property named \"{0}\"", propertyName);
			return nullptr;
		}

		return &GetBufferProperty(propertyIndex);
	}

	inline const MaterialSettings::BufferValue& MaterialInstance::GetBufferProperty(std::size_t storageBufferIndex) const
	{
		if (const MaterialSettings::BufferValue& storageBufferOverride = GetBufferPropertyOverride(storageBufferIndex); storageBufferOverride.buffer)
			return storageBufferOverride;
		else
			return m_materialSettings.GetBufferProperty(storageBufferIndex).defaultValue;
	}

	inline const MaterialSettings::BufferValue& MaterialInstance::GetBufferPropertyOverride(std::size_t storageBufferIndex) const
	{
		assert(storageBufferIndex < m_bufferOverride.size());
		return m_bufferOverride[storageBufferIndex];
	}

	inline const std::shared_ptr<const Material>& MaterialInstance::GetParentMaterial() const
	{
		return m_parent;
	}

	inline MaterialPassFlags MaterialInstance::GetPassFlags(std::size_t passIndex) const
	{
		assert(passIndex < m_passes.size());
		return m_passes[passIndex].flags;
	}

	inline const std::shared_ptr<TextureAsset>* MaterialInstance::GetTextureProperty(std::string_view propertyName) const
	{
		std::size_t propertyIndex = FindTextureProperty(propertyName);
		if (propertyIndex == MaterialSettings::InvalidPropertyIndex)
		{
			NazaraError("material has no texture property named \"{0}\"", propertyName);
			return nullptr;
		}

		return &GetTextureProperty(propertyIndex);
	}

	inline const std::shared_ptr<TextureAsset>& MaterialInstance::GetTextureProperty(std::size_t textureIndex) const
	{
		if (const std::shared_ptr<TextureAsset>& textureOverride = GetTexturePropertyOverride(textureIndex))
			return textureOverride;
		else
			return m_materialSettings.GetTextureProperty(textureIndex).defaultTexture;
	}

	inline const std::shared_ptr<TextureAsset>& MaterialInstance::GetTexturePropertyOverride(std::size_t textureIndex) const
	{
		assert(textureIndex < m_textureOverride.size());
		return m_textureOverride[textureIndex].texture;
	}

	inline const TextureSamplerInfo* MaterialInstance::GetTextureSamplerProperty(std::string_view propertyName) const
	{
		std::size_t propertyIndex = FindTextureProperty(propertyName);
		if (propertyIndex == MaterialSettings::InvalidPropertyIndex)
		{
			NazaraError("material has no texture property named \"{0}\"", propertyName);
			return nullptr;
		}

		return &GetTextureSamplerProperty(propertyIndex);
	}

	inline const TextureSamplerInfo& MaterialInstance::GetTextureSamplerProperty(std::size_t textureIndex) const
	{
		assert(textureIndex < m_textureOverride.size());
		return m_textureOverride[textureIndex].samplerInfo;
	}

	inline const MaterialSettings::Value* MaterialInstance::GetValueProperty(std::string_view propertyName) const
	{
		std::size_t propertyIndex = FindValueProperty(propertyName);
		if (propertyIndex == MaterialSettings::InvalidPropertyIndex)
		{
			NazaraError("material has no value property named \"{0}\"", propertyName);
			return nullptr;
		}

		return &GetValueProperty(propertyIndex);
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

	inline void MaterialInstance::SetBufferProperty(std::string_view propertyName, const MaterialSettings::BufferValue& value)
	{
		std::size_t propertyIndex = FindBufferProperty(propertyName);
		if (propertyIndex == MaterialSettings::InvalidPropertyIndex)
		{
			NazaraError("material has no buffer property named \"{0}\"", propertyName);
			return;
		}

		return SetBufferProperty(propertyIndex, value);
	}

	inline void MaterialInstance::SetTextureProperty(std::string_view propertyName, std::shared_ptr<TextureAsset> texture)
	{
		std::size_t propertyIndex = FindTextureProperty(propertyName);
		if (propertyIndex == MaterialSettings::InvalidPropertyIndex)
		{
			NazaraError("material has no texture property named \"{0}\"", propertyName);
			return;
		}

		return SetTextureProperty(propertyIndex, std::move(texture));
	}

	inline void MaterialInstance::SetTextureProperty(std::string_view propertyName, std::shared_ptr<TextureAsset> texture, const TextureSamplerInfo& samplerInfo)
	{
		std::size_t propertyIndex = FindTextureProperty(propertyName);
		if (propertyIndex == MaterialSettings::InvalidPropertyIndex)
		{
			NazaraError("material has no texture property named \"{0}\"", propertyName);
			return;
		}

		return SetTextureProperty(propertyIndex, std::move(texture), samplerInfo);
	}

	inline void MaterialInstance::SetTextureSamplerProperty(std::string_view propertyName, const TextureSamplerInfo& samplerInfo)
	{
		std::size_t propertyIndex = FindTextureProperty(propertyName);
		if (propertyIndex == MaterialSettings::InvalidPropertyIndex)
		{
			NazaraError("material has no texture property named \"{0}\"", propertyName);
			return;
		}

		return SetTextureSamplerProperty(propertyIndex, samplerInfo);
	}

	inline void MaterialInstance::SetValueProperty(std::string_view propertyName, const MaterialSettings::Value& value)
	{
		std::size_t propertyIndex = FindValueProperty(propertyName);
		if (propertyIndex == MaterialSettings::InvalidPropertyIndex)
		{
			NazaraError("material has no value property named \"{0}\"", propertyName);
			return;
		}

		return SetValueProperty(propertyIndex, value);
	}

	inline void MaterialInstance::UpdatePassFlags(std::size_t passIndex, MaterialPassFlags materialFlags)
	{
		assert(passIndex < m_passes.size());
		if (m_passes[passIndex].flags != materialFlags)
		{
			m_passes[passIndex].flags = materialFlags;
			InvalidatePassPipeline(passIndex);
		}
	}

	template<typename F>
	void MaterialInstance::UpdatePassStates(std::size_t passIndex, F&& stateUpdater)
	{
		assert(passIndex < m_passes.size());
		using Ret = std::invoke_result_t<F, RenderStates&>;
		if constexpr (std::is_same_v<Ret, bool>)
		{
			if (!stateUpdater(static_cast<RenderStates&>(m_passes[passIndex].pipelineInfo)))
				return;
		}
		else if constexpr (std::is_void_v<Ret>)
			stateUpdater(static_cast<RenderStates&>(m_passes[passIndex].pipelineInfo));
		else
			static_assert(AlwaysFalse<Ret>(), "callback must either return a bool or nothing");

		InvalidatePassPipeline(passIndex);
	}

	template<typename F>
	void MaterialInstance::UpdatePassesStates(std::initializer_list<std::size_t> passesIndex, F&& stateUpdater)
	{
		for (std::size_t passIndex : passesIndex)
			UpdatePassStates(passIndex, stateUpdater);
	}

	template<typename F>
	void MaterialInstance::UpdatePassesStates(F&& stateUpdater, bool ignoreDisabled)
	{
		for (std::size_t i = 0; i < m_passes.size(); ++i)
		{
			if (ignoreDisabled && !m_passes[i].enabled)
				continue;

			UpdatePassStates(i, stateUpdater);
		}
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
