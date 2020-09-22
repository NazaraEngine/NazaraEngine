// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline const TextureRef& BasicMaterial::GetAlphaMap() const
	{
		NazaraAssert(HasAlphaMap(), "Material has no alpha map slot");
		return m_material->GetTexture(m_textureIndexes.alpha);
	}

	inline const TextureRef& BasicMaterial::GetDiffuseMap() const
	{
		NazaraAssert(HasDiffuseMap(), "Material has no alpha map slot");
		return m_material->GetTexture(m_textureIndexes.diffuse);
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

	inline bool BasicMaterial::SetAlphaMap(const String& textureName)
	{
		TextureRef texture = TextureLibrary::Query(textureName);
		if (!texture)
		{
			texture = TextureManager::Get(textureName);
			if (!texture)
			{
				NazaraError("Failed to get alpha map \"" + textureName + "\"");
				return false;
			}
		}

		SetAlphaMap(std::move(texture));
		return true;
	}

	inline void BasicMaterial::SetAlphaMap(TextureRef alphaMap)
	{
		NazaraAssert(HasAlphaMap(), "Material has no alpha map slot");
		m_material->SetTexture(m_textureIndexes.alpha, std::move(alphaMap));
	}

	inline bool BasicMaterial::SetDiffuseMap(const String& textureName)
	{
		TextureRef texture = TextureLibrary::Query(textureName);
		if (!texture)
		{
			texture = TextureManager::Get(textureName);
			if (!texture)
			{
				NazaraError("Failed to get diffuse map \"" + textureName + "\"");
				return false;
			}
		}

		SetDiffuseMap(std::move(texture));
		return true;
	}

	inline void BasicMaterial::SetDiffuseMap(TextureRef diffuseMap)
	{
		NazaraAssert(HasDiffuseMap(), "Material has no diffuse map slot");
		m_material->SetTexture(m_textureIndexes.diffuse, std::move(diffuseMap));
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
