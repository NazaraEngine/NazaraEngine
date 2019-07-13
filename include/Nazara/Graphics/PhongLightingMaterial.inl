// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/PhongLightingMaterial.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline const TextureRef& PhongLightingMaterial::GetAlphaMap() const
	{
		NazaraAssert(HasAlphaMap(), "Material has no alpha map slot");
		return m_material->GetTexture(m_textureIndexes.alpha);
	}

	inline const TextureRef& PhongLightingMaterial::GetDiffuseMap() const
	{
		NazaraAssert(HasDiffuseMap(), "Material has no alpha map slot");
		return m_material->GetTexture(m_textureIndexes.diffuse);
	}

	inline const TextureRef& PhongLightingMaterial::GetEmissiveMap() const
	{
		NazaraAssert(HasEmissiveMap(), "Material has no alpha map slot");
		return m_material->GetTexture(m_textureIndexes.emissive);
	}

	inline const TextureRef& PhongLightingMaterial::GetHeightMap() const
	{
		NazaraAssert(HasHeightMap(), "Material has no alpha map slot");
		return m_material->GetTexture(m_textureIndexes.height);
	}

	inline const TextureRef& PhongLightingMaterial::GetNormalMap() const
	{
		NazaraAssert(HasNormalMap(), "Material has no alpha map slot");
		return m_material->GetTexture(m_textureIndexes.normal);
	}

	inline const TextureRef& PhongLightingMaterial::GetSpecularMap() const
	{
		NazaraAssert(HasSpecularMap(), "Material has no alpha map slot");
		return m_material->GetTexture(m_textureIndexes.specular);
	}

	inline bool PhongLightingMaterial::HasAlphaMap() const
	{
		return m_textureIndexes.alpha != MaterialSettings::InvalidIndex;
	}

	inline bool PhongLightingMaterial::HasAlphaThreshold() const
	{
		return m_phongUniformOffsets.alphaThreshold != MaterialSettings::InvalidIndex;
	}

	inline bool PhongLightingMaterial::HasAmbientColor() const
	{
		return m_phongUniformOffsets.ambientColor != MaterialSettings::InvalidIndex;
	}

	inline bool PhongLightingMaterial::HasDiffuseColor() const
	{
		return m_phongUniformOffsets.diffuseColor != MaterialSettings::InvalidIndex;
	}

	inline bool PhongLightingMaterial::HasDiffuseMap() const
	{
		return m_textureIndexes.diffuse != MaterialSettings::InvalidIndex;
	}

	inline bool PhongLightingMaterial::HasEmissiveMap() const
	{
		return m_textureIndexes.emissive != MaterialSettings::InvalidIndex;
	}

	inline bool PhongLightingMaterial::HasHeightMap() const
	{
		return m_textureIndexes.height != MaterialSettings::InvalidIndex;
	}

	inline bool PhongLightingMaterial::HasNormalMap() const
	{
		return m_textureIndexes.normal != MaterialSettings::InvalidIndex;
	}

	inline bool PhongLightingMaterial::HasShininess() const
	{
		return m_phongUniformOffsets.shininess != MaterialSettings::InvalidIndex;
	}

	inline bool PhongLightingMaterial::HasSpecularColor() const
	{
		return m_phongUniformOffsets.specularColor != MaterialSettings::InvalidIndex;
	}

	inline bool PhongLightingMaterial::HasSpecularMap() const
	{
		return m_textureIndexes.specular != MaterialSettings::InvalidIndex;
	}

	inline void PhongLightingMaterial::SetAlphaMap(TextureRef alphaMap)
	{
		NazaraAssert(HasAlphaMap(), "Material has no alpha map slot");
		m_material->SetTexture(m_textureIndexes.alpha, std::move(alphaMap));
	}

	inline bool PhongLightingMaterial::SetDiffuseMap(const String& textureName)
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

	inline void PhongLightingMaterial::SetDiffuseMap(TextureRef diffuseMap)
	{
		NazaraAssert(HasDiffuseMap(), "Material has no diffuse map slot");
		m_material->SetTexture(m_textureIndexes.diffuse, std::move(diffuseMap));
	}

	inline bool PhongLightingMaterial::SetNormalMap(const String& textureName)
	{
		TextureRef texture = TextureLibrary::Query(textureName);
		if (!texture)
		{
			texture = TextureManager::Get(textureName);
			if (!texture)
			{
				NazaraError("Failed to get normal map \"" + textureName + "\"");
				return false;
			}
		}

		SetNormalMap(std::move(texture));
		return true;
	}

	inline void PhongLightingMaterial::SetNormalMap(TextureRef normalMap)
	{
		NazaraAssert(HasNormalMap(), "Material has no normal map slot");
		m_material->SetTexture(m_textureIndexes.normal, std::move(normalMap));
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
