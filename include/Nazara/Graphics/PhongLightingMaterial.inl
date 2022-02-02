// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/PhongLightingMaterial.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline const std::shared_ptr<Texture>& PhongLightingMaterial::GetEmissiveMap() const
	{
		NazaraAssert(HasEmissiveMap(), "Material has no emissive map slot");
		return GetMaterial().GetTexture(m_phongTextureIndexes.emissive);
	}

	inline const TextureSamplerInfo& PhongLightingMaterial::GetEmissiveSampler() const
	{
		NazaraAssert(HasSpecularMap(), "Material has no emissive map slot");
		return GetMaterial().GetTextureSampler(m_phongTextureIndexes.emissive);
	}

	inline const std::shared_ptr<Texture>& PhongLightingMaterial::GetHeightMap() const
	{
		NazaraAssert(HasHeightMap(), "Material has no height map slot");
		return GetMaterial().GetTexture(m_phongTextureIndexes.height);
	}

	inline const TextureSamplerInfo& PhongLightingMaterial::GetHeightSampler() const
	{
		NazaraAssert(HasSpecularMap(), "Material has no height map slot");
		return GetMaterial().GetTextureSampler(m_phongTextureIndexes.height);
	}

	inline const std::shared_ptr<Texture>& PhongLightingMaterial::GetNormalMap() const
	{
		NazaraAssert(HasNormalMap(), "Material has no normal map slot");
		return GetMaterial().GetTexture(m_phongTextureIndexes.normal);
	}

	inline const TextureSamplerInfo& PhongLightingMaterial::GetNormalSampler() const
	{
		NazaraAssert(HasSpecularMap(), "Material has no normal map slot");
		return GetMaterial().GetTextureSampler(m_phongTextureIndexes.normal);
	}

	inline const std::shared_ptr<Texture>& PhongLightingMaterial::GetSpecularMap() const
	{
		NazaraAssert(HasSpecularMap(), "Material has no specular map slot");
		return GetMaterial().GetTexture(m_phongTextureIndexes.specular);
	}

	inline const TextureSamplerInfo& PhongLightingMaterial::GetSpecularSampler() const
	{
		NazaraAssert(HasSpecularMap(), "Material has no specular map slot");
		return GetMaterial().GetTextureSampler(m_phongTextureIndexes.specular);
	}

	inline bool PhongLightingMaterial::HasAmbientColor() const
	{
		return m_phongUniformOffsets.ambientColor != MaterialSettings::InvalidIndex;
	}

	inline bool PhongLightingMaterial::HasEmissiveMap() const
	{
		return m_phongTextureIndexes.emissive != MaterialSettings::InvalidIndex;
	}

	inline bool PhongLightingMaterial::HasHeightMap() const
	{
		return m_phongTextureIndexes.height != MaterialSettings::InvalidIndex;
	}

	inline bool PhongLightingMaterial::HasNormalMap() const
	{
		return m_phongTextureIndexes.normal != MaterialSettings::InvalidIndex;
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
		return m_phongTextureIndexes.specular != MaterialSettings::InvalidIndex;
	}

	inline void PhongLightingMaterial::SetEmissiveMap(std::shared_ptr<Texture> emissiveMap)
	{
		NazaraAssert(HasEmissiveMap(), "Material has no emissive map slot");
		bool hasEmissiveMap = (emissiveMap != nullptr);
		GetMaterial().SetTexture(m_phongTextureIndexes.emissive, std::move(emissiveMap));

		if (m_phongOptionIndexes.hasEmissiveMap != MaterialSettings::InvalidIndex)
			GetMaterial().SetOptionValue(m_phongOptionIndexes.hasEmissiveMap, hasEmissiveMap);
	}

	inline void PhongLightingMaterial::SetEmissiveSampler(TextureSamplerInfo emissiveSampler)
	{
		NazaraAssert(HasEmissiveMap(), "Material has no emissive map slot");
		GetMaterial().SetTextureSampler(m_phongTextureIndexes.emissive, std::move(emissiveSampler));
	}

	inline void PhongLightingMaterial::SetHeightMap(std::shared_ptr<Texture> heightMap)
	{
		NazaraAssert(HasHeightMap(), "Material has no specular map slot");
		bool hasHeightMap = (heightMap != nullptr);
		GetMaterial().SetTexture(m_phongTextureIndexes.height, std::move(heightMap));

		if (m_phongOptionIndexes.hasHeightMap != MaterialSettings::InvalidIndex)
			GetMaterial().SetOptionValue(m_phongOptionIndexes.hasHeightMap, hasHeightMap);
	}

	inline void PhongLightingMaterial::SetHeightSampler(TextureSamplerInfo heightSampler)
	{
		NazaraAssert(HasHeightMap(), "Material has no height map slot");
		GetMaterial().SetTextureSampler(m_phongTextureIndexes.height, std::move(heightSampler));
	}

	inline void PhongLightingMaterial::SetNormalMap(std::shared_ptr<Texture> normalMap)
	{
		NazaraAssert(HasNormalMap(), "Material has no normal map slot");
		bool hasNormalMap = (normalMap != nullptr);
		GetMaterial().SetTexture(m_phongTextureIndexes.normal, std::move(normalMap));

		if (m_phongOptionIndexes.hasNormalMap != MaterialSettings::InvalidIndex)
			GetMaterial().SetOptionValue(m_phongOptionIndexes.hasNormalMap, hasNormalMap);
	}

	inline void PhongLightingMaterial::SetNormalSampler(TextureSamplerInfo normalSampler)
	{
		NazaraAssert(HasNormalMap(), "Material has no normal map slot");
		GetMaterial().SetTextureSampler(m_phongTextureIndexes.normal, std::move(normalSampler));
	}

	inline void PhongLightingMaterial::SetSpecularMap(std::shared_ptr<Texture> specularMap)
	{
		NazaraAssert(HasNormalMap(), "Material has no specular map slot");
		bool hasSpecularMap = (specularMap != nullptr);
		GetMaterial().SetTexture(m_phongTextureIndexes.specular, std::move(specularMap));

		if (m_phongOptionIndexes.hasSpecularMap != MaterialSettings::InvalidIndex)
			GetMaterial().SetOptionValue(m_phongOptionIndexes.hasSpecularMap, hasSpecularMap);
	}

	inline void PhongLightingMaterial::SetSpecularSampler(TextureSamplerInfo specularSampler)
	{
		NazaraAssert(HasSpecularMap(), "Material has no specular map slot");
		GetMaterial().SetTextureSampler(m_phongTextureIndexes.specular, std::move(specularSampler));
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
