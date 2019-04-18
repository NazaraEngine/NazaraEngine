// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/PhongLightingMaterial.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Utility/MaterialData.hpp>
#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace
	{
		constexpr std::size_t AlphaMapBinding = 0;
		constexpr std::size_t DiffuseMapBinding = 1;
		constexpr std::size_t EmissiveMapBinding = 2;
		constexpr std::size_t HeightMapBinding = 3;
		constexpr std::size_t NormalMapBinding = 4;
		constexpr std::size_t SpecularMapBinding = 5;
	}

	inline PhongLightingMaterial::PhongLightingMaterial(Material* material)
	{
		NazaraAssert(material, "Invalid material");

		// Most common case: don't fetch texture indexes as a little optimization
		const std::shared_ptr<const MaterialSettings>& materialSettings = material->GetSettings();
		if (materialSettings == s_materialSettings)
			m_textureIndexes = s_textureIndexes;
		else
		{
			m_textureIndexes.alpha = materialSettings->GetTextureIndex("alpha");
			m_textureIndexes.diffuse = materialSettings->GetTextureIndex("height");
			m_textureIndexes.emissive = materialSettings->GetTextureIndex("emissive");
			m_textureIndexes.height = materialSettings->GetTextureIndex("height");
			m_textureIndexes.normal = materialSettings->GetTextureIndex("normal");
			m_textureIndexes.specular = materialSettings->GetTextureIndex("specular");
		}
	}

	bool PhongLightingMaterial::Initialize()
	{
		RenderPipelineLayoutInfo info;
		info.bindings.assign({
			{
				"MaterialAlphaMap",
				ShaderBindingType_Texture,
				ShaderStageType_Fragment,
				AlphaMapBinding
			},
			{
				"MaterialDiffuseMap",
				ShaderBindingType_Texture,
				ShaderStageType_Fragment,
				DiffuseMapBinding
			},
			{
				"MaterialEmissiveMap",
				ShaderBindingType_Texture,
				ShaderStageType_Fragment,
				EmissiveMapBinding
			},
			{
				"MaterialHeightMap",
				ShaderBindingType_Texture,
				ShaderStageType_Fragment,
				HeightMapBinding
			},
			{
				"MaterialNormalMap",
				ShaderBindingType_Texture,
				ShaderStageType_Fragment,
				NormalMapBinding
			},
			{
				"MaterialSpecularMap",
				ShaderBindingType_Texture,
				ShaderStageType_Fragment,
				SpecularMapBinding
			}
		});

		s_renderPipelineLayout = RenderPipelineLayout::New();
		s_renderPipelineLayout->Create(info);

		s_materialSettings = std::make_shared<MaterialSettings>();

		s_textureIndexes.alpha = s_materialSettings->textures.size();
		s_materialSettings->textures.push_back({
			"Alpha",
			ImageType_2D,
			"MaterialAlphaMap"
		});
		
		s_textureIndexes.diffuse = s_materialSettings->textures.size();
		s_materialSettings->textures.push_back({
			"Diffuse",
			ImageType_2D,
			"MaterialDiffuseMap"
		});

		s_textureIndexes.emissive = s_materialSettings->textures.size();
		s_materialSettings->textures.push_back({
			"Emissive",
			ImageType_2D,
			"MaterialEmissiveMap"
		});

		s_textureIndexes.height = s_materialSettings->textures.size();
		s_materialSettings->textures.push_back({
			"Height",
			ImageType_2D,
			"MaterialHeightMap"
		});

		s_textureIndexes.normal = s_materialSettings->textures.size();
		s_materialSettings->textures.push_back({
			"Normal",
			ImageType_2D,
			"MaterialNormalMap"
		});

		s_textureIndexes.specular = s_materialSettings->textures.size();
		s_materialSettings->textures.push_back({
			"Specular",
			ImageType_2D,
			"MaterialSpecularMap"
		});

		return true;
	}

	void PhongLightingMaterial::Uninitialize()
	{
		s_renderPipelineLayout.Reset();
		s_materialSettings.reset();
	}

	std::shared_ptr<MaterialSettings> PhongLightingMaterial::s_materialSettings;
	RenderPipelineLayoutRef PhongLightingMaterial::s_renderPipelineLayout;
	PhongLightingMaterial::TextureIndexes PhongLightingMaterial::s_textureIndexes;
}
