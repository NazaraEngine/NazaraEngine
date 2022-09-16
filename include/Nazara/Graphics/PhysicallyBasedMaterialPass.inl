// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/PhysicallyBasedMaterialPass.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline const std::shared_ptr<Texture>& PhysicallyBasedMaterialPass::GetEmissiveMap() const
	{
		NazaraAssert(HasEmissiveMap(), "Material has no emissive map slot");
		return GetMaterialPass().GetTexture(m_pbrTextureIndexes.emissive);
	}

	inline const TextureSamplerInfo& PhysicallyBasedMaterialPass::GetEmissiveSampler() const
	{
		NazaraAssert(HasSpecularMap(), "Material has no emissive map slot");
		return GetMaterialPass().GetTextureSampler(m_pbrTextureIndexes.emissive);
	}

	inline const std::shared_ptr<Texture>& PhysicallyBasedMaterialPass::GetHeightMap() const
	{
		NazaraAssert(HasHeightMap(), "Material has no height map slot");
		return GetMaterialPass().GetTexture(m_pbrTextureIndexes.height);
	}

	inline const TextureSamplerInfo& PhysicallyBasedMaterialPass::GetHeightSampler() const
	{
		NazaraAssert(HasSpecularMap(), "Material has no height map slot");
		return GetMaterialPass().GetTextureSampler(m_pbrTextureIndexes.height);
	}

	inline const std::shared_ptr<Texture>& PhysicallyBasedMaterialPass::GetMetallicMap() const
	{
		NazaraAssert(HasMetallicMap(), "Material has no metallic map slot");
		return GetMaterialPass().GetTexture(m_pbrTextureIndexes.metallic);
	}

	inline const TextureSamplerInfo& PhysicallyBasedMaterialPass::GetMetallicSampler() const
	{
		NazaraAssert(HasMetallicMap(), "Material has no metallic map slot");
		return GetMaterialPass().GetTextureSampler(m_pbrTextureIndexes.metallic);
	}

	inline const std::shared_ptr<Texture>& PhysicallyBasedMaterialPass::GetNormalMap() const
	{
		NazaraAssert(HasNormalMap(), "Material has no normal map slot");
		return GetMaterialPass().GetTexture(m_pbrTextureIndexes.normal);
	}

	inline const TextureSamplerInfo& PhysicallyBasedMaterialPass::GetNormalSampler() const
	{
		NazaraAssert(HasSpecularMap(), "Material has no normal map slot");
		return GetMaterialPass().GetTextureSampler(m_pbrTextureIndexes.normal);
	}

	inline const std::shared_ptr<Texture>& PhysicallyBasedMaterialPass::GetRoughnessMap() const
	{
		NazaraAssert(HasRoughnessMap(), "Material has no roughness map slot");
		return GetMaterialPass().GetTexture(m_pbrTextureIndexes.roughness);
	}

	inline const TextureSamplerInfo& PhysicallyBasedMaterialPass::GetRoughnessSampler() const
	{
		NazaraAssert(HasRoughnessMap(), "Material has no roughness map slot");
		return GetMaterialPass().GetTextureSampler(m_pbrTextureIndexes.roughness);
	}

	inline const std::shared_ptr<Texture>& PhysicallyBasedMaterialPass::GetSpecularMap() const
	{
		NazaraAssert(HasSpecularMap(), "Material has no specular map slot");
		return GetMaterialPass().GetTexture(m_pbrTextureIndexes.specular);
	}

	inline const TextureSamplerInfo& PhysicallyBasedMaterialPass::GetSpecularSampler() const
	{
		NazaraAssert(HasSpecularMap(), "Material has no specular map slot");
		return GetMaterialPass().GetTextureSampler(m_pbrTextureIndexes.specular);
	}

	inline bool PhysicallyBasedMaterialPass::HasAmbientColor() const
	{
		return m_pbrUniformOffsets.ambientColor != MaterialSettings::InvalidIndex;
	}

	inline bool PhysicallyBasedMaterialPass::HasEmissiveMap() const
	{
		return m_pbrTextureIndexes.emissive != MaterialSettings::InvalidIndex;
	}

	inline bool PhysicallyBasedMaterialPass::HasHeightMap() const
	{
		return m_pbrTextureIndexes.height != MaterialSettings::InvalidIndex;
	}

	inline bool PhysicallyBasedMaterialPass::HasMetallicMap() const
	{
		return m_pbrTextureIndexes.metallic != MaterialSettings::InvalidIndex;
	}

	inline bool PhysicallyBasedMaterialPass::HasNormalMap() const
	{
		return m_pbrTextureIndexes.normal != MaterialSettings::InvalidIndex;
	}

	inline bool PhysicallyBasedMaterialPass::HasRoughnessMap() const
	{
		return m_pbrTextureIndexes.roughness != MaterialSettings::InvalidIndex;
	}

	inline bool PhysicallyBasedMaterialPass::HasShininess() const
	{
		return m_pbrUniformOffsets.shininess != MaterialSettings::InvalidIndex;
	}

	inline bool PhysicallyBasedMaterialPass::HasSpecularColor() const
	{
		return m_pbrUniformOffsets.specularColor != MaterialSettings::InvalidIndex;
	}

	inline bool PhysicallyBasedMaterialPass::HasSpecularMap() const
	{
		return m_pbrTextureIndexes.specular != MaterialSettings::InvalidIndex;
	}

	inline void PhysicallyBasedMaterialPass::SetEmissiveMap(std::shared_ptr<Texture> emissiveMap)
	{
		NazaraAssert(HasEmissiveMap(), "Material has no emissive map slot");
		bool hasEmissiveMap = (emissiveMap != nullptr);
		GetMaterialPass().SetTexture(m_pbrTextureIndexes.emissive, std::move(emissiveMap));

		if (m_pbrOptionIndexes.hasEmissiveMap != MaterialSettings::InvalidIndex)
			GetMaterialPass().SetOptionValue(m_pbrOptionIndexes.hasEmissiveMap, hasEmissiveMap);
	}

	inline void PhysicallyBasedMaterialPass::SetEmissiveSampler(TextureSamplerInfo emissiveSampler)
	{
		NazaraAssert(HasEmissiveMap(), "Material has no emissive map slot");
		GetMaterialPass().SetTextureSampler(m_pbrTextureIndexes.emissive, std::move(emissiveSampler));
	}

	inline void PhysicallyBasedMaterialPass::SetHeightMap(std::shared_ptr<Texture> heightMap)
	{
		NazaraAssert(HasHeightMap(), "Material has no specular map slot");
		bool hasHeightMap = (heightMap != nullptr);
		GetMaterialPass().SetTexture(m_pbrTextureIndexes.height, std::move(heightMap));

		if (m_pbrOptionIndexes.hasHeightMap != MaterialSettings::InvalidIndex)
			GetMaterialPass().SetOptionValue(m_pbrOptionIndexes.hasHeightMap, hasHeightMap);
	}

	inline void PhysicallyBasedMaterialPass::SetHeightSampler(TextureSamplerInfo heightSampler)
	{
		NazaraAssert(HasHeightMap(), "Material has no height map slot");
		GetMaterialPass().SetTextureSampler(m_pbrTextureIndexes.height, std::move(heightSampler));
	}

	inline void PhysicallyBasedMaterialPass::SetMetallicMap(std::shared_ptr<Texture> metallicMap)
	{
		NazaraAssert(HasMetallicMap(), "Material has no metallic map slot");
		bool hasMetallicMap = (metallicMap != nullptr);
		GetMaterialPass().SetTexture(m_pbrTextureIndexes.metallic, std::move(metallicMap));

		if (m_pbrOptionIndexes.hasMetallicMap != MaterialSettings::InvalidIndex)
			GetMaterialPass().SetOptionValue(m_pbrOptionIndexes.hasMetallicMap, hasMetallicMap);
	}

	inline void PhysicallyBasedMaterialPass::SetMetallicSampler(TextureSamplerInfo metallicSampler)
	{
		NazaraAssert(HasMetallicMap(), "Material has no metallic map slot");
		GetMaterialPass().SetTextureSampler(m_pbrTextureIndexes.metallic, std::move(metallicSampler));
	}

	inline void PhysicallyBasedMaterialPass::SetNormalMap(std::shared_ptr<Texture> normalMap)
	{
		NazaraAssert(HasNormalMap(), "Material has no normal map slot");
		bool hasNormalMap = (normalMap != nullptr);
		GetMaterialPass().SetTexture(m_pbrTextureIndexes.normal, std::move(normalMap));

		if (m_pbrOptionIndexes.hasNormalMap != MaterialSettings::InvalidIndex)
			GetMaterialPass().SetOptionValue(m_pbrOptionIndexes.hasNormalMap, hasNormalMap);
	}

	inline void PhysicallyBasedMaterialPass::SetNormalSampler(TextureSamplerInfo normalSampler)
	{
		NazaraAssert(HasNormalMap(), "Material has no normal map slot");
		GetMaterialPass().SetTextureSampler(m_pbrTextureIndexes.normal, std::move(normalSampler));
	}

	inline void PhysicallyBasedMaterialPass::SetRoughnessMap(std::shared_ptr<Texture> roughnessMap)
	{
		NazaraAssert(HasRoughnessMap(), "Material has no roughness map slot");
		bool hasRoughnessMap = (roughnessMap != nullptr);
		GetMaterialPass().SetTexture(m_pbrTextureIndexes.roughness, std::move(roughnessMap));

		if (m_pbrOptionIndexes.hasRoughnessMap != MaterialSettings::InvalidIndex)
			GetMaterialPass().SetOptionValue(m_pbrOptionIndexes.hasRoughnessMap, hasRoughnessMap);
	}

	inline void PhysicallyBasedMaterialPass::SetRoughnessSampler(TextureSamplerInfo metallicSampler)
	{
		NazaraAssert(HasRoughnessMap(), "Material has no roughness map slot");
		GetMaterialPass().SetTextureSampler(m_pbrTextureIndexes.roughness, std::move(metallicSampler));
	}

	inline void PhysicallyBasedMaterialPass::SetSpecularMap(std::shared_ptr<Texture> specularMap)
	{
		NazaraAssert(HasNormalMap(), "Material has no specular map slot");
		bool hasSpecularMap = (specularMap != nullptr);
		GetMaterialPass().SetTexture(m_pbrTextureIndexes.specular, std::move(specularMap));

		if (m_pbrOptionIndexes.hasSpecularMap != MaterialSettings::InvalidIndex)
			GetMaterialPass().SetOptionValue(m_pbrOptionIndexes.hasSpecularMap, hasSpecularMap);
	}

	inline void PhysicallyBasedMaterialPass::SetSpecularSampler(TextureSamplerInfo specularSampler)
	{
		NazaraAssert(HasSpecularMap(), "Material has no specular map slot");
		GetMaterialPass().SetTextureSampler(m_pbrTextureIndexes.specular, std::move(specularSampler));
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
