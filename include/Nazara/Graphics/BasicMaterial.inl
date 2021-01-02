// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline void BasicMaterial::EnableAlphaTest(bool alphaTest)
	{
		NazaraAssert(HasAlphaTest(), "Material has no alpha test condition");
		m_material.EnableCondition(m_conditionIndexes.alphaTest, alphaTest);
	}

	inline const std::shared_ptr<Texture>& BasicMaterial::GetAlphaMap() const
	{
		NazaraAssert(HasAlphaMap(), "Material has no alpha texture slot");
		return m_material.GetTexture(m_textureIndexes.alpha);
	}

	inline const std::shared_ptr<TextureSampler>& BasicMaterial::GetAlphaSampler() const
	{
		NazaraAssert(HasAlphaMap(), "Material has no alpha texture slot");
		return m_material.GetTextureSampler(m_textureIndexes.alpha);
	}

	inline const std::shared_ptr<Texture>& BasicMaterial::GetDiffuseMap() const
	{
		NazaraAssert(HasDiffuseMap(), "Material has no alpha texture slot");
		return m_material.GetTexture(m_textureIndexes.diffuse);
	}

	inline const std::shared_ptr<TextureSampler>& BasicMaterial::GetDiffuseSampler() const
	{
		NazaraAssert(HasDiffuseMap(), "Material has no alpha texture slot");
		return m_material.GetTextureSampler(m_textureIndexes.diffuse);
	}

	inline bool BasicMaterial::HasAlphaMap() const
	{
		return m_textureIndexes.alpha != MaterialSettings::InvalidIndex;
	}

	inline bool BasicMaterial::HasAlphaTest() const
	{
		return m_conditionIndexes.alphaTest != MaterialSettings::InvalidIndex;
	}

	inline bool BasicMaterial::HasAlphaTestThreshold() const
	{
		return m_uniformOffsets.alphaThreshold != MaterialSettings::InvalidIndex;
	}

	inline bool BasicMaterial::HasDiffuseColor() const
	{
		return m_uniformOffsets.diffuseColor != MaterialSettings::InvalidIndex;
	}

	inline bool BasicMaterial::HasDiffuseMap() const
	{
		return m_textureIndexes.diffuse != MaterialSettings::InvalidIndex;
	}

	inline void BasicMaterial::SetAlphaMap(std::shared_ptr<Texture> alphaMap)
	{
		NazaraAssert(HasAlphaMap(), "Material has no alpha map slot");
		bool hasAlphaMap = (alphaMap != nullptr);
		m_material.SetTexture(m_textureIndexes.alpha, std::move(alphaMap));

		if (m_conditionIndexes.hasDiffuseMap != MaterialSettings::InvalidIndex)
			m_material.EnableCondition(m_conditionIndexes.hasAlphaMap, hasAlphaMap);
	}

	inline void BasicMaterial::SetAlphaSampler(std::shared_ptr<TextureSampler> alphaSampler)
	{
		NazaraAssert(HasAlphaMap(), "Material has no alpha map slot");
		m_material.SetTextureSampler(m_textureIndexes.alpha, std::move(alphaSampler));
	}

	inline void BasicMaterial::SetDiffuseMap(std::shared_ptr<Texture> diffuseMap)
	{
		NazaraAssert(HasDiffuseMap(), "Material has no diffuse map slot");
		bool hasDiffuseMap = (diffuseMap != nullptr);
		m_material.SetTexture(m_textureIndexes.diffuse, std::move(diffuseMap));

		if (m_conditionIndexes.hasDiffuseMap != MaterialSettings::InvalidIndex)
			m_material.EnableCondition(m_conditionIndexes.hasDiffuseMap, hasDiffuseMap);
	}

	inline void BasicMaterial::SetDiffuseSampler(std::shared_ptr<TextureSampler> diffuseSampler)
	{
		NazaraAssert(HasDiffuseMap(), "Material has no diffuse map slot");
		m_material.SetTextureSampler(m_textureIndexes.diffuse, std::move(diffuseSampler));
	}

	inline const std::shared_ptr<MaterialSettings>& BasicMaterial::GetSettings()
	{
		return s_materialSettings;
	}

	inline auto BasicMaterial::GetOffsets() -> const UniformOffsets&
	{
		return s_uniformOffsets;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
