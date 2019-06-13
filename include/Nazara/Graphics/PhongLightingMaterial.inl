// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/PhongLightingMaterial.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline const Texture* PhongLightingMaterial::GetAlphaMap() const
	{
		NazaraAssert(HasAlphaMap(), "Material has no alpha map slot");
		return m_material->GetTexture(m_textureIndexes.alpha);
	}

	inline const Texture* PhongLightingMaterial::GetDiffuseMap() const
	{
		NazaraAssert(HasDiffuseMap(), "Material has no alpha map slot");
		return m_material->GetTexture(m_textureIndexes.diffuse);
	}

	inline const Texture* PhongLightingMaterial::GetEmissiveMap() const
	{
		NazaraAssert(HasEmissiveMap(), "Material has no alpha map slot");
		return m_material->GetTexture(m_textureIndexes.emissive);
	}

	inline const Texture* PhongLightingMaterial::GetHeightMap() const
	{
		NazaraAssert(HasHeightMap(), "Material has no alpha map slot");
		return m_material->GetTexture(m_textureIndexes.height);
	}

	inline const Texture* PhongLightingMaterial::GetNormalMap() const
	{
		NazaraAssert(HasNormalMap(), "Material has no alpha map slot");
		return m_material->GetTexture(m_textureIndexes.normal);
	}

	inline const Texture* PhongLightingMaterial::GetSpecularMap() const
	{
		NazaraAssert(HasSpecularMap(), "Material has no alpha map slot");
		return m_material->GetTexture(m_textureIndexes.specular);
	}

	inline bool PhongLightingMaterial::HasAlphaMap() const
	{
		return m_textureIndexes.alpha != MaterialSettings::InvalidIndex;
	}

	inline void PhongLightingMaterial::SetAlphaMap(TextureRef alphaMap)
	{
		NazaraAssert(HasAlphaMap(), "Material has no alpha map slot");
		m_material->SetTexture(m_textureIndexes.alpha, std::move(alphaMap));
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
