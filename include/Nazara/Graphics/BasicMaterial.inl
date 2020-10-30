// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline const std::shared_ptr<Texture>& BasicMaterial::GetAlphaMap() const
	{
		NazaraAssert(HasAlphaMap(), "Material has no alpha map slot");
		return m_material.GetTexture(m_textureIndexes.alpha);
	}

	inline const std::shared_ptr<Texture>& BasicMaterial::GetDiffuseMap() const
	{
		NazaraAssert(HasDiffuseMap(), "Material has no alpha map slot");
		return m_material.GetTexture(m_textureIndexes.diffuse);
	}

	inline bool BasicMaterial::HasAlphaMap() const
	{
		return m_textureIndexes.alpha != MaterialSettings::InvalidIndex;
	}

	inline bool BasicMaterial::HasAlphaThreshold() const
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
		m_material.SetTexture(m_textureIndexes.alpha, std::move(alphaMap));
	}

	inline void BasicMaterial::SetDiffuseMap(std::shared_ptr<Texture> diffuseMap)
	{
		NazaraAssert(HasDiffuseMap(), "Material has no diffuse map slot");
		m_material.SetTexture(m_textureIndexes.diffuse, std::move(diffuseMap));
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
