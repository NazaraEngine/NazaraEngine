// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/PhongLightingMaterial.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/MaterialData.hpp>
#include <NZSL/Parser.hpp>
#include <NZSL/Math/FieldOffsets.hpp>
#include <cassert>
#include <filesystem>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	PhongLightingMaterial::PhongLightingMaterial(MaterialPass& material) :
	BasicMaterial(material, NoInit{})
	{
		// Most common case: don't fetch texture indexes as a little optimization
		const std::shared_ptr<const MaterialSettings>& materialSettings = GetMaterial().GetSettings();
		if (materialSettings == s_phongMaterialSettings)
		{
			m_basicUniformOffsets = s_basicUniformOffsets;
			m_basicOptionIndexes = s_basicOptionIndexes;
			m_basicTextureIndexes = s_basicTextureIndexes;

			m_phongOptionIndexes = s_phongOptionIndexes;
			m_phongTextureIndexes = s_phongTextureIndexes;
			m_phongUniformOffsets = s_phongUniformOffsets;
		}
		else
		{
			m_basicOptionIndexes.alphaTest = materialSettings->GetOptionIndex("AlphaTest");
			m_basicOptionIndexes.hasAlphaMap = materialSettings->GetOptionIndex("HasAlphaMap");
			m_basicOptionIndexes.hasBaseColorMap = materialSettings->GetOptionIndex("HasBaseColorMap");

			m_phongOptionIndexes.hasEmissiveMap = materialSettings->GetOptionIndex("HasEmissiveMap");
			m_phongOptionIndexes.hasHeightMap = materialSettings->GetOptionIndex("HasHeightMap");
			m_phongOptionIndexes.hasNormalMap = materialSettings->GetOptionIndex("HasNormalMap");
			m_phongOptionIndexes.hasSpecularMap = materialSettings->GetOptionIndex("HasSpecularMap");

			m_basicTextureIndexes.alpha = materialSettings->GetTextureIndex("Alpha");
			m_basicTextureIndexes.baseColor = materialSettings->GetTextureIndex("BaseColor");

			m_phongTextureIndexes.emissive = materialSettings->GetTextureIndex("Emissive");
			m_phongTextureIndexes.height = materialSettings->GetTextureIndex("Height");
			m_phongTextureIndexes.normal = materialSettings->GetTextureIndex("Normal");
			m_phongTextureIndexes.specular = materialSettings->GetTextureIndex("Specular");

			m_uniformBlockIndex = materialSettings->GetUniformBlockIndex("MaterialSettings");
			if (m_uniformBlockIndex != MaterialSettings::InvalidIndex)
			{
				m_basicUniformOffsets.alphaThreshold = materialSettings->GetUniformBlockVariableOffset(m_uniformBlockIndex, "AlphaThreshold");
				m_basicUniformOffsets.baseColor = materialSettings->GetUniformBlockVariableOffset(m_uniformBlockIndex, "BaseColor");

				m_phongUniformOffsets.ambientColor = materialSettings->GetUniformBlockVariableOffset(m_uniformBlockIndex, "AmbientColor");
				m_phongUniformOffsets.shininess = materialSettings->GetUniformBlockVariableOffset(m_uniformBlockIndex, "Shininess");
				m_phongUniformOffsets.specularColor = materialSettings->GetUniformBlockVariableOffset(m_uniformBlockIndex, "SpecularColor");
			}
			else
			{
				m_basicUniformOffsets.alphaThreshold = MaterialSettings::InvalidIndex;
				m_basicUniformOffsets.baseColor = MaterialSettings::InvalidIndex;

				m_phongUniformOffsets.ambientColor = MaterialSettings::InvalidIndex;
				m_phongUniformOffsets.shininess = MaterialSettings::InvalidIndex;
				m_phongUniformOffsets.specularColor = MaterialSettings::InvalidIndex;
			}
		}
	}

	Color PhongLightingMaterial::GetAmbientColor() const
	{
		NazaraAssert(HasAmbientColor(), "Material has no ambient color uniform");

		const std::vector<UInt8>& bufferData = GetMaterial().GetUniformBufferConstData(m_uniformBlockIndex);

		const float* colorPtr = AccessByOffset<const float*>(bufferData.data(), m_phongUniformOffsets.ambientColor);
		return Color(colorPtr[0], colorPtr[1], colorPtr[2], colorPtr[3]);
	}

	float Nz::PhongLightingMaterial::GetShininess() const
	{
		NazaraAssert(HasShininess(), "Material has no shininess uniform");

		const std::vector<UInt8>& bufferData = GetMaterial().GetUniformBufferConstData(m_uniformBlockIndex);
		return AccessByOffset<const float&>(bufferData.data(), m_phongUniformOffsets.shininess);
	}

	Color PhongLightingMaterial::GetSpecularColor() const
	{
		NazaraAssert(HasSpecularColor(), "Material has no specular color uniform");

		const std::vector<UInt8>& bufferData = GetMaterial().GetUniformBufferConstData(m_uniformBlockIndex);

		const float* colorPtr = AccessByOffset<const float*>(bufferData.data(), m_phongUniformOffsets.specularColor);
		return Color(colorPtr[0], colorPtr[1], colorPtr[2], colorPtr[3]);
	}

	void PhongLightingMaterial::SetAmbientColor(const Color& ambient)
	{
		NazaraAssert(HasAmbientColor(), "Material has no ambient color uniform");

		std::vector<UInt8>& bufferData = GetMaterial().GetUniformBufferData(m_uniformBlockIndex);
		float* colorPtr = AccessByOffset<float*>(bufferData.data(), m_phongUniformOffsets.ambientColor);
		colorPtr[0] = ambient.r;
		colorPtr[1] = ambient.g;
		colorPtr[2] = ambient.b;
		colorPtr[3] = ambient.a;
	}

	void PhongLightingMaterial::SetShininess(float shininess)
	{
		NazaraAssert(HasShininess(), "Material has no shininess uniform");

		std::vector<UInt8>& bufferData = GetMaterial().GetUniformBufferData(m_uniformBlockIndex);
		AccessByOffset<float&>(bufferData.data(), m_phongUniformOffsets.shininess) = shininess;
	}

	void PhongLightingMaterial::SetSpecularColor(const Color& specular)
	{
		NazaraAssert(HasSpecularColor(), "Material has no specular color uniform");

		std::vector<UInt8>& bufferData = GetMaterial().GetUniformBufferData(m_uniformBlockIndex);
		float* colorPtr = AccessByOffset<float*>(bufferData.data(), m_phongUniformOffsets.specularColor);
		colorPtr[0] = specular.r;
		colorPtr[1] = specular.g;
		colorPtr[2] = specular.b;
		colorPtr[3] = specular.a;
	}

	const std::shared_ptr<MaterialSettings>& PhongLightingMaterial::GetSettings()
	{
		return s_phongMaterialSettings;
	}

	MaterialSettings::Builder PhongLightingMaterial::Build(PhongBuildOptions& options)
	{
		MaterialSettings::Builder settings = BasicMaterial::Build(options);

		assert(settings.uniformBlocks.size() == 1);
		std::vector<MaterialSettings::UniformVariable> variables = std::move(settings.uniformBlocks.front().uniforms);
		settings.uniformBlocks.clear();

		if (options.phongOffsets.ambientColor != std::numeric_limits<std::size_t>::max())
		{
			variables.push_back({
				"AmbientColor",
				options.phongOffsets.ambientColor
			});
		}

		if (options.phongOffsets.shininess != std::numeric_limits<std::size_t>::max())
		{
			variables.push_back({
				"Shininess",
				options.phongOffsets.shininess
			});
		}

		if (options.phongOffsets.shininess != std::numeric_limits<std::size_t>::max())
		{
			variables.push_back({
				"SpecularColor",
				options.phongOffsets.specularColor
			});
		}

		static_assert(sizeof(Vector4f) == 4 * sizeof(float), "Vector4f is expected to be exactly 4 floats wide");

		if (options.phongOffsets.ambientColor != std::numeric_limits<std::size_t>::max())
			AccessByOffset<Vector4f&>(options.defaultValues.data(), options.phongOffsets.ambientColor) = Vector4f(0.f, 0.f, 0.f, 1.f);

		if (options.phongOffsets.specularColor != std::numeric_limits<std::size_t>::max())
			AccessByOffset<Vector4f&>(options.defaultValues.data(), options.phongOffsets.specularColor) = Vector4f(1.f, 1.f, 1.f, 1.f);

		if (options.phongOffsets.shininess != std::numeric_limits<std::size_t>::max())
			AccessByOffset<float&>(options.defaultValues.data(), options.phongOffsets.shininess) = 2.f;

		// Textures
		if (options.phongTextureIndexes)
			options.phongTextureIndexes->emissive = settings.textures.size();

		settings.textures.push_back({
			7,
			"Emissive",
			ImageType::E2D
		});

		if (options.phongTextureIndexes)
			options.phongTextureIndexes->height = settings.textures.size();

		settings.textures.push_back({
			8,
			"Height",
			ImageType::E2D
		});

		if (options.phongTextureIndexes)
			options.phongTextureIndexes->normal = settings.textures.size();

		settings.textures.push_back({
			9,
			"Normal",
			ImageType::E2D
		});

		if (options.phongTextureIndexes)
			options.phongTextureIndexes->specular = settings.textures.size();

		settings.textures.push_back({
			10,
			"Specular",
			ImageType::E2D
		});

		if (options.uniformBlockIndex)
			*options.uniformBlockIndex = settings.uniformBlocks.size();

		settings.uniformBlocks.push_back({
			0,
			"MaterialSettings",
			options.phongOffsets.totalSize,
			std::move(variables),
			options.defaultValues
		});

		settings.sharedUniformBlocks.push_back(PredefinedLightData::GetUniformBlock(6, nzsl::ShaderStageType::Fragment));
		settings.predefinedBindings[UnderlyingCast(PredefinedShaderBinding::LightDataUbo)] = 6;

		settings.shaders = options.shaders;

		for (const std::shared_ptr<UberShader>& uberShader : settings.shaders)
		{
			uberShader->UpdateConfigCallback([=](UberShader::Config& config, const std::vector<RenderPipelineInfo::VertexBufferData>& vertexBuffers)
			{
				if (vertexBuffers.empty())
					return;

				const VertexDeclaration& vertexDeclaration = *vertexBuffers.front().declaration;
				const auto& components = vertexDeclaration.GetComponents();

				Int32 locationIndex = 0;
				for (const auto& component : components)
				{
					switch (component.component)
					{
						case VertexComponent::Position:
							config.optionValues[CRC32("PosLocation")] = locationIndex;
							break;

						case VertexComponent::Color:
							config.optionValues[CRC32("ColorLocation")] = locationIndex;
							break;

						case VertexComponent::Normal:
							config.optionValues[CRC32("NormalLocation")] = locationIndex;
							break;

						case VertexComponent::Tangent:
							config.optionValues[CRC32("TangentLocation")] = locationIndex;
							break;

						case VertexComponent::TexCoord:
							config.optionValues[CRC32("UvLocation")] = locationIndex;
							break;

						case VertexComponent::Unused:
						default:
							break;
					}

					++locationIndex;
				}
			});
		}

		// Options

		// HasEmissiveMap
		if (options.phongOptionIndexes)
			options.phongOptionIndexes->hasEmissiveMap = settings.options.size();

		MaterialSettings::BuildOption(settings.options, "HasEmissiveMap", "HasEmissiveTexture");

		// HasHeightMap
		if (options.phongOptionIndexes)
			options.phongOptionIndexes->hasHeightMap = settings.options.size();

		MaterialSettings::BuildOption(settings.options, "HasHeightMap", "HasHeightTexture");

		// HasNormalMap
		if (options.phongOptionIndexes)
			options.phongOptionIndexes->hasNormalMap = settings.options.size();

		MaterialSettings::BuildOption(settings.options, "HasNormalMap", "HasNormalTexture");

		// HasSpecularMap
		if (options.phongOptionIndexes)
			options.phongOptionIndexes->hasSpecularMap = settings.options.size();

		MaterialSettings::BuildOption(settings.options, "HasSpecularMap", "HasSpecularTexture");

		return settings;
	}

	std::vector<std::shared_ptr<UberShader>> PhongLightingMaterial::BuildShaders()
	{
		auto shader = std::make_shared<UberShader>(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, "PhongMaterial");

		return { std::move(shader) };
	}

	auto PhongLightingMaterial::BuildUniformOffsets() -> std::pair<PhongUniformOffsets, nzsl::FieldOffsets>
	{
		auto basicOffsets = BasicMaterial::BuildUniformOffsets();
		nzsl::FieldOffsets fieldOffsets = basicOffsets.second;

		PhongUniformOffsets uniformOffsets;
		uniformOffsets.ambientColor = fieldOffsets.AddField(nzsl::StructFieldType::Float3);
		uniformOffsets.specularColor = fieldOffsets.AddField(nzsl::StructFieldType::Float3);
		uniformOffsets.shininess = fieldOffsets.AddField(nzsl::StructFieldType::Float1);

		uniformOffsets.totalSize = fieldOffsets.GetAlignedSize();

		return std::make_pair(std::move(uniformOffsets), std::move(fieldOffsets));
	}

	bool PhongLightingMaterial::Initialize()
	{
		std::tie(s_phongUniformOffsets, std::ignore) = BuildUniformOffsets();

		std::vector<UInt8> defaultValues(s_phongUniformOffsets.totalSize);

		PhongBuildOptions options;
		options.defaultValues = std::move(defaultValues);
		options.shaders = BuildShaders();

		// Basic material
		options.basicOffsets = s_basicUniformOffsets;

		// Phong Material
		options.phongOffsets = s_phongUniformOffsets;
		options.phongOptionIndexes = &s_phongOptionIndexes;
		options.phongTextureIndexes = &s_phongTextureIndexes;

		s_phongMaterialSettings = std::make_shared<MaterialSettings>(Build(options));

		return true;
	}

	void PhongLightingMaterial::Uninitialize()
	{
		s_phongMaterialSettings.reset();
	}

	std::shared_ptr<MaterialSettings> PhongLightingMaterial::s_phongMaterialSettings;
	std::size_t PhongLightingMaterial::s_phongUniformBlockIndex;
	PhongLightingMaterial::PhongOptionIndexes PhongLightingMaterial::s_phongOptionIndexes;
	PhongLightingMaterial::PhongTextureIndexes PhongLightingMaterial::s_phongTextureIndexes;
	PhongLightingMaterial::PhongUniformOffsets PhongLightingMaterial::s_phongUniformOffsets;
}
