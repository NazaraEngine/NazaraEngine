// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/PhysicallyBasedMaterial.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline const std::shared_ptr<Texture>& PhysicallyBasedMaterial::GetEmissiveMap() const
	{
		NazaraAssert(HasEmissiveMap(), "Material has no emissive map slot");
		return GetMaterial().GetTexture(m_pbrTextureIndexes.emissive);
	}

	inline const TextureSamplerInfo& PhysicallyBasedMaterial::GetEmissiveSampler() const
	{
		NazaraAssert(HasSpecularMap(), "Material has no emissive map slot");
		return GetMaterial().GetTextureSampler(m_pbrTextureIndexes.emissive);
	}

	inline const std::shared_ptr<Texture>& PhysicallyBasedMaterial::GetHeightMap() const
	{
		NazaraAssert(HasHeightMap(), "Material has no height map slot");
		return GetMaterial().GetTexture(m_pbrTextureIndexes.height);
	}

	inline const TextureSamplerInfo& PhysicallyBasedMaterial::GetHeightSampler() const
	{
		NazaraAssert(HasSpecularMap(), "Material has no height map slot");
		return GetMaterial().GetTextureSampler(m_pbrTextureIndexes.height);
	}

	inline const std::shared_ptr<Texture>& PhysicallyBasedMaterial::GetMetallicMap() const
	{
		NazaraAssert(HasMetallicMap(), "Material has no metallic map slot");
		return GetMaterial().GetTexture(m_pbrTextureIndexes.metallic);
	}

	inline const TextureSamplerInfo& PhysicallyBasedMaterial::GetMetallicSampler() const
	{
		NazaraAssert(HasMetallicMap(), "Material has no metallic map slot");
		return GetMaterial().GetTextureSampler(m_pbrTextureIndexes.metallic);
	}

	inline const std::shared_ptr<Texture>& PhysicallyBasedMaterial::GetNormalMap() const
	{
		NazaraAssert(HasNormalMap(), "Material has no normal map slot");
		return GetMaterial().GetTexture(m_pbrTextureIndexes.normal);
	}

	inline const TextureSamplerInfo& PhysicallyBasedMaterial::GetNormalSampler() const
	{
		NazaraAssert(HasSpecularMap(), "Material has no normal map slot");
		return GetMaterial().GetTextureSampler(m_pbrTextureIndexes.normal);
	}

	inline const std::shared_ptr<Texture>& PhysicallyBasedMaterial::GetRoughnessMap() const
	{
		NazaraAssert(HasRoughnessMap(), "Material has no roughness map slot");
		return GetMaterial().GetTexture(m_pbrTextureIndexes.roughness);
	}

	inline const TextureSamplerInfo& PhysicallyBasedMaterial::GetRoughnessSampler() const
	{
		NazaraAssert(HasRoughnessMap(), "Material has no roughness map slot");
		return GetMaterial().GetTextureSampler(m_pbrTextureIndexes.roughness);
	}

	inline const std::shared_ptr<Texture>& PhysicallyBasedMaterial::GetSpecularMap() const
	{
		NazaraAssert(HasSpecularMap(), "Material has no specular map slot");
		return GetMaterial().GetTexture(m_pbrTextureIndexes.specular);
	}

	inline const TextureSamplerInfo& PhysicallyBasedMaterial::GetSpecularSampler() const
	{
		NazaraAssert(HasSpecularMap(), "Material has no specular map slot");
		return GetMaterial().GetTextureSampler(m_pbrTextureIndexes.specular);
	}

	inline bool PhysicallyBasedMaterial::HasAmbientColor() const
	{
		return m_pbrUniformOffsets.ambientColor != MaterialSettings::InvalidIndex;
	}

	inline bool PhysicallyBasedMaterial::HasEmissiveMap() const
	{
		return m_pbrTextureIndexes.emissive != MaterialSettings::InvalidIndex;
	}

	inline bool PhysicallyBasedMaterial::HasHeightMap() const
	{
		return m_pbrTextureIndexes.height != MaterialSettings::InvalidIndex;
	}

	inline bool PhysicallyBasedMaterial::HasMetallicMap() const
	{
		return m_pbrTextureIndexes.metallic != MaterialSettings::InvalidIndex;
	}

	inline bool PhysicallyBasedMaterial::HasNormalMap() const
	{
		return m_pbrTextureIndexes.normal != MaterialSettings::InvalidIndex;
	}

	inline bool PhysicallyBasedMaterial::HasRoughnessMap() const
	{
		return m_pbrTextureIndexes.roughness != MaterialSettings::InvalidIndex;
	}

	inline bool PhysicallyBasedMaterial::HasShininess() const
	{
		return m_pbrUniformOffsets.shininess != MaterialSettings::InvalidIndex;
	}

	inline bool PhysicallyBasedMaterial::HasSpecularColor() const
	{
		return m_pbrUniformOffsets.specularColor != MaterialSettings::InvalidIndex;
	}

	inline bool PhysicallyBasedMaterial::HasSpecularMap() const
	{
		return m_pbrTextureIndexes.specular != MaterialSettings::InvalidIndex;
	}

	inline void PhysicallyBasedMaterial::SetEmissiveMap(std::shared_ptr<Texture> emissiveMap)
	{
		NazaraAssert(HasEmissiveMap(), "Material has no emissive map slot");
		bool hasEmissiveMap = (emissiveMap != nullptr);
		GetMaterial().SetTexture(m_pbrTextureIndexes.emissive, std::move(emissiveMap));

		if (m_pbrOptionIndexes.hasEmissiveMap != MaterialSettings::InvalidIndex)
			GetMaterial().SetOptionValue(m_pbrOptionIndexes.hasEmissiveMap, hasEmissiveMap);
	}

	inline void PhysicallyBasedMaterial::SetEmissiveSampler(TextureSamplerInfo emissiveSampler)
	{
		NazaraAssert(HasEmissiveMap(), "Material has no emissive map slot");
		GetMaterial().SetTextureSampler(m_pbrTextureIndexes.emissive, std::move(emissiveSampler));
	}

	inline void PhysicallyBasedMaterial::SetHeightMap(std::shared_ptr<Texture> heightMap)
	{
		NazaraAssert(HasHeightMap(), "Material has no specular map slot");
		bool hasHeightMap = (heightMap != nullptr);
		GetMaterial().SetTexture(m_pbrTextureIndexes.height, std::move(heightMap));

		if (m_pbrOptionIndexes.hasHeightMap != MaterialSettings::InvalidIndex)
			GetMaterial().SetOptionValue(m_pbrOptionIndexes.hasHeightMap, hasHeightMap);
	}

	inline void PhysicallyBasedMaterial::SetHeightSampler(TextureSamplerInfo heightSampler)
	{
		NazaraAssert(HasHeightMap(), "Material has no height map slot");
		GetMaterial().SetTextureSampler(m_pbrTextureIndexes.height, std::move(heightSampler));
	}

	inline void PhysicallyBasedMaterial::SetMetallicMap(std::shared_ptr<Texture> metallicMap)
	{
		NazaraAssert(HasMetallicMap(), "Material has no metallic map slot");
		bool hasMetallicMap = (metallicMap != nullptr);
		GetMaterial().SetTexture(m_pbrTextureIndexes.metallic, std::move(metallicMap));

		if (m_pbrOptionIndexes.hasMetallicMap != MaterialSettings::InvalidIndex)
			GetMaterial().SetOptionValue(m_pbrOptionIndexes.hasMetallicMap, hasMetallicMap);
	}

	inline void PhysicallyBasedMaterial::SetMetallicSampler(TextureSamplerInfo metallicSampler)
	{
		NazaraAssert(HasMetallicMap(), "Material has no metallic map slot");
		GetMaterial().SetTextureSampler(m_pbrTextureIndexes.metallic, std::move(metallicSampler));
	}

	inline void PhysicallyBasedMaterial::SetNormalMap(std::shared_ptr<Texture> normalMap)
	{
		NazaraAssert(HasNormalMap(), "Material has no normal map slot");
		bool hasNormalMap = (normalMap != nullptr);
		GetMaterial().SetTexture(m_pbrTextureIndexes.normal, std::move(normalMap));

		if (m_pbrOptionIndexes.hasNormalMap != MaterialSettings::InvalidIndex)
			GetMaterial().SetOptionValue(m_pbrOptionIndexes.hasNormalMap, hasNormalMap);
	}

	inline void PhysicallyBasedMaterial::SetNormalSampler(TextureSamplerInfo normalSampler)
	{
		NazaraAssert(HasNormalMap(), "Material has no normal map slot");
		GetMaterial().SetTextureSampler(m_pbrTextureIndexes.normal, std::move(normalSampler));
	}

	inline void PhysicallyBasedMaterial::SetRoughnessMap(std::shared_ptr<Texture> roughnessMap)
	{
		NazaraAssert(HasRoughnessMap(), "Material has no roughness map slot");
		bool hasRoughnessMap = (roughnessMap != nullptr);
		GetMaterial().SetTexture(m_pbrTextureIndexes.roughness, std::move(roughnessMap));

		if (m_pbrOptionIndexes.hasRoughnessMap != MaterialSettings::InvalidIndex)
			GetMaterial().SetOptionValue(m_pbrOptionIndexes.hasRoughnessMap, hasRoughnessMap);
	}

	inline void PhysicallyBasedMaterial::SetRoughnessSampler(TextureSamplerInfo metallicSampler)
	{
		NazaraAssert(HasRoughnessMap(), "Material has no roughness map slot");
		GetMaterial().SetTextureSampler(m_pbrTextureIndexes.roughness, std::move(metallicSampler));
	}

	inline void PhysicallyBasedMaterial::SetSpecularMap(std::shared_ptr<Texture> specularMap)
	{
		NazaraAssert(HasNormalMap(), "Material has no specular map slot");
		bool hasSpecularMap = (specularMap != nullptr);
		GetMaterial().SetTexture(m_pbrTextureIndexes.specular, std::move(specularMap));

		if (m_pbrOptionIndexes.hasSpecularMap != MaterialSettings::InvalidIndex)
			GetMaterial().SetOptionValue(m_pbrOptionIndexes.hasSpecularMap, hasSpecularMap);
	}

	inline void PhysicallyBasedMaterial::SetSpecularSampler(TextureSamplerInfo specularSampler)
	{
		NazaraAssert(HasSpecularMap(), "Material has no specular map slot");
		GetMaterial().SetTextureSampler(m_pbrTextureIndexes.specular, std::move(specularSampler));
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
