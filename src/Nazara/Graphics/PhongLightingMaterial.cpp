// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/PhongLightingMaterial.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/FieldOffsets.hpp>
#include <Nazara/Utility/MaterialData.hpp>
#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	PhongLightingMaterial::PhongLightingMaterial(Material& material) :
	m_material(material)
	{
		// Most common case: don't fetch texture indexes as a little optimization
		const std::shared_ptr<const MaterialSettings>& materialSettings = m_material.GetSettings();
		if (materialSettings == s_materialSettings)
		{
			m_textureIndexes = s_textureIndexes;
			m_phongUniformIndex = s_phongUniformBlockIndex;
			m_phongUniformOffsets = s_phongUniformOffsets;
		}
		else
		{
			m_textureIndexes.alpha = materialSettings->GetTextureIndex("Alpha");
			m_textureIndexes.diffuse = materialSettings->GetTextureIndex("Diffuse");
			m_textureIndexes.emissive = materialSettings->GetTextureIndex("Emissive");
			m_textureIndexes.height = materialSettings->GetTextureIndex("Height");
			m_textureIndexes.normal = materialSettings->GetTextureIndex("Normal");
			m_textureIndexes.specular = materialSettings->GetTextureIndex("Specular");

			m_phongUniformIndex = materialSettings->GetUniformBlockIndex("PhongSettings");

			m_phongUniformOffsets.alphaThreshold = materialSettings->GetUniformBlockVariableOffset(m_phongUniformIndex, "AlphaThreshold");
			m_phongUniformOffsets.ambientColor = materialSettings->GetUniformBlockVariableOffset(m_phongUniformIndex, "AmbientColor");
			m_phongUniformOffsets.diffuseColor = materialSettings->GetUniformBlockVariableOffset(m_phongUniformIndex, "DiffuseColor");
			m_phongUniformOffsets.shininess = materialSettings->GetUniformBlockVariableOffset(m_phongUniformIndex, "Shininess");
			m_phongUniformOffsets.specularColor = materialSettings->GetUniformBlockVariableOffset(m_phongUniformIndex, "SpecularColor");
		}
	}

	float PhongLightingMaterial::GetAlphaThreshold() const
	{
		NazaraAssert(HasAlphaThreshold(), "Material has no alpha threshold uniform");

		BufferMapper<UniformBuffer> mapper(m_material.GetUniformBuffer(m_phongUniformIndex), BufferAccess_ReadOnly);
		return AccessByOffset<const float&>(mapper.GetPointer(), m_phongUniformOffsets.alphaThreshold);
	}

	Color PhongLightingMaterial::GetAmbientColor() const
	{
		NazaraAssert(HasAmbientColor(), "Material has no ambient color uniform");

		BufferMapper<UniformBuffer> mapper(m_material.GetUniformBuffer(m_phongUniformIndex), BufferAccess_ReadOnly);

		const float* colorPtr = AccessByOffset<const float*>(mapper.GetPointer(), m_phongUniformOffsets.ambientColor);
		return Color(colorPtr[0] * 255, colorPtr[1] * 255, colorPtr[2] * 255, colorPtr[3] * 255); //< TODO: Make color able to use float
	}

	Color PhongLightingMaterial::GetDiffuseColor() const
	{
		NazaraAssert(HasDiffuseColor(), "Material has no diffuse color uniform");

		BufferMapper<UniformBuffer> mapper(m_material.GetUniformBuffer(m_phongUniformIndex), BufferAccess_ReadOnly);

		const float* colorPtr = AccessByOffset<const float*>(mapper.GetPointer(), m_phongUniformOffsets.diffuseColor);
		return Color(colorPtr[0] * 255, colorPtr[1] * 255, colorPtr[2] * 255, colorPtr[3] * 255); //< TODO: Make color able to use float
	}

	float Nz::PhongLightingMaterial::GetShininess() const
	{
		NazaraAssert(HasShininess(), "Material has no shininess uniform");

		BufferMapper<UniformBuffer> mapper(m_material.GetUniformBuffer(m_phongUniformIndex), BufferAccess_ReadOnly);
		return AccessByOffset<const float&>(mapper.GetPointer(), m_phongUniformOffsets.shininess);
	}

	Color PhongLightingMaterial::GetSpecularColor() const
	{
		NazaraAssert(HasSpecularColor(), "Material has no specular color uniform");

		BufferMapper<UniformBuffer> mapper(m_material.GetUniformBuffer(m_phongUniformIndex), BufferAccess_ReadOnly);

		const float* colorPtr = AccessByOffset<const float*>(mapper.GetPointer(), m_phongUniformOffsets.specularColor);
		return Color(colorPtr[0] * 255, colorPtr[1] * 255, colorPtr[2] * 255, colorPtr[3] * 255); //< TODO: Make color able to use float
	}

	void PhongLightingMaterial::SetAlphaThreshold(float alphaThreshold)
	{
		NazaraAssert(HasAlphaThreshold(), "Material has no alpha threshold uniform");

		BufferMapper<UniformBuffer> mapper(m_material.GetUniformBuffer(m_phongUniformIndex), BufferAccess_WriteOnly);
		AccessByOffset<float&>(mapper.GetPointer(), m_phongUniformOffsets.alphaThreshold) = alphaThreshold;
	}

	void PhongLightingMaterial::SetAmbientColor(const Color& ambient)
	{
		NazaraAssert(HasAmbientColor(), "Material has no ambient color uniform");

		BufferMapper<UniformBuffer> mapper(m_material.GetUniformBuffer(m_phongUniformIndex), BufferAccess_WriteOnly);
		float* colorPtr = AccessByOffset<float*>(mapper.GetPointer(), m_phongUniformOffsets.ambientColor);
		colorPtr[0] = ambient.r / 255.f;
		colorPtr[1] = ambient.g / 255.f;
		colorPtr[2] = ambient.b / 255.f;
		colorPtr[3] = ambient.a / 255.f;
	}

	void PhongLightingMaterial::SetDiffuseColor(const Color& diffuse)
	{
		NazaraAssert(HasDiffuseColor(), "Material has no diffuse color uniform");

		BufferMapper<UniformBuffer> mapper(m_material.GetUniformBuffer(m_phongUniformIndex), BufferAccess_WriteOnly);
		float* colorPtr = AccessByOffset<float*>(mapper.GetPointer(), m_phongUniformOffsets.diffuseColor);
		colorPtr[0] = diffuse.r / 255.f;
		colorPtr[1] = diffuse.g / 255.f;
		colorPtr[2] = diffuse.b / 255.f;
		colorPtr[3] = diffuse.a / 255.f;
	}

	void PhongLightingMaterial::SetSpecularColor(const Color& diffuse)
	{
		NazaraAssert(HasSpecularColor(), "Material has no specular color uniform");

		BufferMapper<UniformBuffer> mapper(m_material.GetUniformBuffer(m_phongUniformIndex), BufferAccess_WriteOnly);
		float* colorPtr = AccessByOffset<float*>(mapper.GetPointer(), m_phongUniformOffsets.specularColor);
		colorPtr[0] = diffuse.r / 255.f;
		colorPtr[1] = diffuse.g / 255.f;
		colorPtr[2] = diffuse.b / 255.f;
		colorPtr[3] = diffuse.a / 255.f;
	}

	const std::shared_ptr<MaterialSettings>& PhongLightingMaterial::GetSettings()
	{
		return s_materialSettings;
	}

	bool PhongLightingMaterial::Initialize()
	{
		// MaterialPhongSettings
		FieldOffsets phongUniformStruct(StructLayout_Std140);

		s_phongUniformOffsets.alphaThreshold = phongUniformStruct.AddField(StructFieldType_Float1);
		s_phongUniformOffsets.shininess = phongUniformStruct.AddField(StructFieldType_Float1);
		s_phongUniformOffsets.ambientColor = phongUniformStruct.AddField(StructFieldType_Float4);
		s_phongUniformOffsets.diffuseColor = phongUniformStruct.AddField(StructFieldType_Float4);
		s_phongUniformOffsets.specularColor = phongUniformStruct.AddField(StructFieldType_Float4);

		MaterialSettings::Builder settings;
		settings.predefinedBinding.fill(MaterialSettings::InvalidIndex);

		std::vector<MaterialSettings::UniformVariable> phongVariables;
		phongVariables.assign({
			{
				"AlphaThreshold",
				s_phongUniformOffsets.alphaThreshold
			},
			{
				"Shininess",
				s_phongUniformOffsets.shininess
			},
			{
				"AmbientColor",
				s_phongUniformOffsets.ambientColor
			},
			{
				"DiffuseColor",
				s_phongUniformOffsets.diffuseColor
			},
			{
				"SpecularColor",
				s_phongUniformOffsets.specularColor
			}
		});

		static_assert(sizeof(Vector4f) == 4 * sizeof(float), "Vector4f is expected to be exactly 4 floats wide");

		std::vector<UInt8> defaultValues(phongUniformStruct.GetSize());
		AccessByOffset<Vector4f&>(defaultValues.data(), s_phongUniformOffsets.ambientColor) = Vector4f(0.5f, 0.5f, 0.5f, 1.f);
		AccessByOffset<Vector4f&>(defaultValues.data(), s_phongUniformOffsets.diffuseColor) = Vector4f(1.f, 1.f, 1.f, 1.f);
		AccessByOffset<Vector4f&>(defaultValues.data(), s_phongUniformOffsets.specularColor) = Vector4f(1.f, 1.f, 1.f, 1.f);
		AccessByOffset<float&>(defaultValues.data(), s_phongUniformOffsets.alphaThreshold) = 0.2f;
		AccessByOffset<float&>(defaultValues.data(), s_phongUniformOffsets.shininess) = 50.f;

		s_phongUniformBlockIndex = settings.uniformBlocks.size();
		settings.uniformBlocks.push_back({
			phongUniformStruct.GetSize(),
			"PhongSettings",
			"MaterialPhongSettings",
			std::move(phongVariables),
			std::move(defaultValues)
		});

		settings.predefinedBinding[UnderlyingCast(PredefinedShaderBinding::UboInstanceData)] = settings.sharedUniformBlocks.size();
		settings.sharedUniformBlocks.push_back(PredefinedInstanceData::GetUniformBlock());
		settings.predefinedBinding[UnderlyingCast(PredefinedShaderBinding::UboLighData)] = settings.sharedUniformBlocks.size();
		settings.sharedUniformBlocks.push_back(PredefinedLightData::GetUniformBlock());
		settings.predefinedBinding[UnderlyingCast(PredefinedShaderBinding::UboViewerData)] = settings.sharedUniformBlocks.size();
		settings.sharedUniformBlocks.push_back(PredefinedViewerData::GetUniformBlock());

		s_textureIndexes.alpha = settings.textures.size();
		settings.textures.push_back({
			"MaterialAlphaMap",
			"Alpha",
			ImageType_2D
		});
		
		s_textureIndexes.diffuse = settings.textures.size();
		settings.textures.push_back({
			"MaterialDiffuseMap",
			"Diffuse",
			ImageType_2D
		});

		s_textureIndexes.emissive = settings.textures.size();
		settings.textures.push_back({
			"MaterialEmissiveMap",
			"Emissive",
			ImageType_2D
		});

		s_textureIndexes.height = settings.textures.size();
		settings.textures.push_back({
			"MaterialHeightMap",
			"Height",
			ImageType_2D
		});

		s_textureIndexes.normal = settings.textures.size();
		settings.textures.push_back({
			"MaterialNormalMap",
			"Normal",
			ImageType_2D
		});

		s_textureIndexes.specular = settings.textures.size();
		settings.textures.push_back({
			"MaterialSpecularMap",
			"Specular",
			ImageType_2D
		});

		settings.predefinedBinding[UnderlyingCast(PredefinedShaderBinding::TexOverlay)] = settings.textures.size();
		settings.textures.push_back({
			"TextureOverlay",
			"Overlay",
			ImageType_2D,
		});

		s_materialSettings = std::make_shared<MaterialSettings>(std::move(settings));

		return true;
	}

	void PhongLightingMaterial::Uninitialize()
	{
		s_materialSettings.reset();
	}

	std::shared_ptr<MaterialSettings> PhongLightingMaterial::s_materialSettings;
	std::size_t PhongLightingMaterial::s_phongUniformBlockIndex;
	PhongLightingMaterial::TextureIndexes PhongLightingMaterial::s_textureIndexes;
	PhongLightingMaterial::PhongUniformOffsets PhongLightingMaterial::s_phongUniformOffsets;
}
