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
	PhongLightingMaterial::PhongLightingMaterial(MaterialPass& material) :
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

		const std::vector<UInt8>& bufferData = m_material.GetUniformBufferConstData(m_phongUniformIndex);
		return AccessByOffset<const float&>(bufferData.data(), m_phongUniformOffsets.alphaThreshold);
	}

	Color PhongLightingMaterial::GetAmbientColor() const
	{
		NazaraAssert(HasAmbientColor(), "Material has no ambient color uniform");

		const std::vector<UInt8>& bufferData = m_material.GetUniformBufferConstData(m_phongUniformIndex);

		const float* colorPtr = AccessByOffset<const float*>(bufferData.data(), m_phongUniformOffsets.ambientColor);
		return Color(colorPtr[0] * 255, colorPtr[1] * 255, colorPtr[2] * 255, colorPtr[3] * 255); //< TODO: Make color able to use float
	}

	Color PhongLightingMaterial::GetDiffuseColor() const
	{
		NazaraAssert(HasDiffuseColor(), "Material has no diffuse color uniform");

		const std::vector<UInt8>& bufferData = m_material.GetUniformBufferConstData(m_phongUniformIndex);

		const float* colorPtr = AccessByOffset<const float*>(bufferData.data(), m_phongUniformOffsets.diffuseColor);
		return Color(colorPtr[0] * 255, colorPtr[1] * 255, colorPtr[2] * 255, colorPtr[3] * 255); //< TODO: Make color able to use float
	}

	float Nz::PhongLightingMaterial::GetShininess() const
	{
		NazaraAssert(HasShininess(), "Material has no shininess uniform");

		const std::vector<UInt8>& bufferData = m_material.GetUniformBufferConstData(m_phongUniformIndex);
		return AccessByOffset<const float&>(bufferData.data(), m_phongUniformOffsets.shininess);
	}

	Color PhongLightingMaterial::GetSpecularColor() const
	{
		NazaraAssert(HasSpecularColor(), "Material has no specular color uniform");

		const std::vector<UInt8>& bufferData = m_material.GetUniformBufferConstData(m_phongUniformIndex);

		const float* colorPtr = AccessByOffset<const float*>(bufferData.data(), m_phongUniformOffsets.specularColor);
		return Color(colorPtr[0] * 255, colorPtr[1] * 255, colorPtr[2] * 255, colorPtr[3] * 255); //< TODO: Make color able to use float
	}

	void PhongLightingMaterial::SetAlphaThreshold(float alphaThreshold)
	{
		NazaraAssert(HasAlphaThreshold(), "Material has no alpha threshold uniform");

		std::vector<UInt8>& bufferData = m_material.GetUniformBufferData(m_phongUniformIndex);
		AccessByOffset<float&>(bufferData.data(), m_phongUniformOffsets.alphaThreshold) = alphaThreshold;
	}

	void PhongLightingMaterial::SetAmbientColor(const Color& ambient)
	{
		NazaraAssert(HasAmbientColor(), "Material has no ambient color uniform");

		std::vector<UInt8>& bufferData = m_material.GetUniformBufferData(m_phongUniformIndex);
		float* colorPtr = AccessByOffset<float*>(bufferData.data(), m_phongUniformOffsets.ambientColor);
		colorPtr[0] = ambient.r / 255.f;
		colorPtr[1] = ambient.g / 255.f;
		colorPtr[2] = ambient.b / 255.f;
		colorPtr[3] = ambient.a / 255.f;
	}

	void PhongLightingMaterial::SetDiffuseColor(const Color& diffuse)
	{
		NazaraAssert(HasDiffuseColor(), "Material has no diffuse color uniform");

		std::vector<UInt8>& bufferData = m_material.GetUniformBufferData(m_phongUniformIndex);
		float* colorPtr = AccessByOffset<float*>(bufferData.data(), m_phongUniformOffsets.diffuseColor);
		colorPtr[0] = diffuse.r / 255.f;
		colorPtr[1] = diffuse.g / 255.f;
		colorPtr[2] = diffuse.b / 255.f;
		colorPtr[3] = diffuse.a / 255.f;
	}

	void PhongLightingMaterial::SetSpecularColor(const Color& diffuse)
	{
		NazaraAssert(HasSpecularColor(), "Material has no specular color uniform");

		std::vector<UInt8>& bufferData = m_material.GetUniformBufferData(m_phongUniformIndex);
		float* colorPtr = AccessByOffset<float*>(bufferData.data(), m_phongUniformOffsets.specularColor);
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
		FieldOffsets phongUniformStruct(StructLayout::Std140);

		s_phongUniformOffsets.alphaThreshold = phongUniformStruct.AddField(StructFieldType::Float1);
		s_phongUniformOffsets.shininess = phongUniformStruct.AddField(StructFieldType::Float1);
		s_phongUniformOffsets.ambientColor = phongUniformStruct.AddField(StructFieldType::Float4);
		s_phongUniformOffsets.diffuseColor = phongUniformStruct.AddField(StructFieldType::Float4);
		s_phongUniformOffsets.specularColor = phongUniformStruct.AddField(StructFieldType::Float4);

		MaterialSettings::Builder settings;

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
			0,
			"PhongSettings",
			phongUniformStruct.GetSize(),
			std::move(phongVariables),
			std::move(defaultValues)
		});

		s_textureIndexes.alpha = settings.textures.size();
		settings.textures.push_back({
			2,
			"Alpha",
			ImageType::E2D
		});
		
		s_textureIndexes.diffuse = settings.textures.size();
		settings.textures.push_back({
			1,
			"Diffuse",
			ImageType::E2D
		});

		s_textureIndexes.emissive = settings.textures.size();
		settings.textures.push_back({
			3,
			"Emissive",
			ImageType::E2D
		});

		s_textureIndexes.height = settings.textures.size();
		settings.textures.push_back({
			4,
			"Height",
			ImageType::E2D
		});

		s_textureIndexes.normal = settings.textures.size();
		settings.textures.push_back({
			5,
			"Normal",
			ImageType::E2D
		});

		s_textureIndexes.specular = settings.textures.size();
		settings.textures.push_back({
			6,
			"Specular",
			ImageType::E2D
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
