// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/PhysicallyBasedMaterial.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/MaterialData.hpp>
#include <NZSL/FieldOffsets.hpp>
#include <NZSL/Parser.hpp>
#include <cassert>
#include <filesystem>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	PhysicallyBasedMaterial::PhysicallyBasedMaterial(MaterialPass& material) :
	BasicMaterial(material, NoInit{})
	{
		// Most common case: don't fetch texture indexes as a little optimization
		const std::shared_ptr<const MaterialSettings>& materialSettings = GetMaterial().GetSettings();
		if (materialSettings == s_pbrMaterialSettings)
		{
			m_basicUniformOffsets = s_basicUniformOffsets;
			m_basicOptionIndexes = s_basicOptionIndexes;
			m_basicTextureIndexes = s_basicTextureIndexes;

			m_pbrOptionIndexes = s_pbrOptionIndexes;
			m_pbrTextureIndexes = s_pbrTextureIndexes;
			m_pbrUniformOffsets = s_pbrUniformOffsets;
		}
		else
		{
			m_basicOptionIndexes.alphaTest = materialSettings->GetOptionIndex("AlphaTest");
			m_basicOptionIndexes.hasAlphaMap = materialSettings->GetOptionIndex("HasAlphaMap");
			m_basicOptionIndexes.hasDiffuseMap = materialSettings->GetOptionIndex("HasDiffuseMap");

			m_pbrOptionIndexes.hasEmissiveMap = materialSettings->GetOptionIndex("HasEmissiveMap");
			m_pbrOptionIndexes.hasHeightMap = materialSettings->GetOptionIndex("HasHeightMap");
			m_pbrOptionIndexes.hasMetallicMap = materialSettings->GetOptionIndex("HasMetallicMap");
			m_pbrOptionIndexes.hasNormalMap = materialSettings->GetOptionIndex("HasNormalMap");
			m_pbrOptionIndexes.hasRoughnessMap = materialSettings->GetOptionIndex("HasRoughnessMap");
			m_pbrOptionIndexes.hasSpecularMap = materialSettings->GetOptionIndex("HasSpecularMap");

			m_basicTextureIndexes.alpha = materialSettings->GetTextureIndex("Alpha");
			m_basicTextureIndexes.diffuse = materialSettings->GetTextureIndex("Diffuse");

			m_pbrTextureIndexes.emissive = materialSettings->GetTextureIndex("Emissive");
			m_pbrTextureIndexes.height = materialSettings->GetTextureIndex("Height");
			m_pbrTextureIndexes.normal = materialSettings->GetTextureIndex("Normal");
			m_pbrTextureIndexes.specular = materialSettings->GetTextureIndex("Specular");

			m_uniformBlockIndex = materialSettings->GetUniformBlockIndex("MaterialSettings");
			if (m_uniformBlockIndex != MaterialSettings::InvalidIndex)
			{
				m_basicUniformOffsets.alphaThreshold = materialSettings->GetUniformBlockVariableOffset(m_uniformBlockIndex, "AlphaThreshold");
				m_basicUniformOffsets.diffuseColor = materialSettings->GetUniformBlockVariableOffset(m_uniformBlockIndex, "DiffuseColor");

				m_pbrUniformOffsets.ambientColor = materialSettings->GetUniformBlockVariableOffset(m_uniformBlockIndex, "AmbientColor");
				m_pbrUniformOffsets.shininess = materialSettings->GetUniformBlockVariableOffset(m_uniformBlockIndex, "Shininess");
				m_pbrUniformOffsets.specularColor = materialSettings->GetUniformBlockVariableOffset(m_uniformBlockIndex, "SpecularColor");
			}
			else
			{
				m_basicUniformOffsets.alphaThreshold = MaterialSettings::InvalidIndex;
				m_basicUniformOffsets.diffuseColor = MaterialSettings::InvalidIndex;

				m_pbrUniformOffsets.ambientColor = MaterialSettings::InvalidIndex;
				m_pbrUniformOffsets.shininess = MaterialSettings::InvalidIndex;
				m_pbrUniformOffsets.specularColor = MaterialSettings::InvalidIndex;
			}
		}
	}

	Color PhysicallyBasedMaterial::GetAmbientColor() const
	{
		NazaraAssert(HasAmbientColor(), "Material has no ambient color uniform");

		const std::vector<UInt8>& bufferData = GetMaterial().GetUniformBufferConstData(m_uniformBlockIndex);

		const float* colorPtr = AccessByOffset<const float*>(bufferData.data(), m_pbrUniformOffsets.ambientColor);
		return Color(colorPtr[0] * 255, colorPtr[1] * 255, colorPtr[2] * 255, colorPtr[3] * 255); //< TODO: Make color able to use float
	}

	float Nz::PhysicallyBasedMaterial::GetShininess() const
	{
		NazaraAssert(HasShininess(), "Material has no shininess uniform");

		const std::vector<UInt8>& bufferData = GetMaterial().GetUniformBufferConstData(m_uniformBlockIndex);
		return AccessByOffset<const float&>(bufferData.data(), m_pbrUniformOffsets.shininess);
	}

	Color PhysicallyBasedMaterial::GetSpecularColor() const
	{
		NazaraAssert(HasSpecularColor(), "Material has no specular color uniform");

		const std::vector<UInt8>& bufferData = GetMaterial().GetUniformBufferConstData(m_uniformBlockIndex);

		const float* colorPtr = AccessByOffset<const float*>(bufferData.data(), m_pbrUniformOffsets.specularColor);
		return Color(colorPtr[0] * 255, colorPtr[1] * 255, colorPtr[2] * 255, colorPtr[3] * 255); //< TODO: Make color able to use float
	}

	void PhysicallyBasedMaterial::SetAmbientColor(const Color& ambient)
	{
		NazaraAssert(HasAmbientColor(), "Material has no ambient color uniform");

		std::vector<UInt8>& bufferData = GetMaterial().GetUniformBufferData(m_uniformBlockIndex);
		float* colorPtr = AccessByOffset<float*>(bufferData.data(), m_pbrUniformOffsets.ambientColor);
		colorPtr[0] = ambient.r / 255.f;
		colorPtr[1] = ambient.g / 255.f;
		colorPtr[2] = ambient.b / 255.f;
		colorPtr[3] = ambient.a / 255.f;
	}

	void PhysicallyBasedMaterial::SetShininess(float shininess)
	{
		NazaraAssert(HasShininess(), "Material has no shininess uniform");

		std::vector<UInt8>& bufferData = GetMaterial().GetUniformBufferData(m_uniformBlockIndex);
		AccessByOffset<float&>(bufferData.data(), m_pbrUniformOffsets.shininess) = shininess;
	}

	void PhysicallyBasedMaterial::SetSpecularColor(const Color& diffuse)
	{
		NazaraAssert(HasSpecularColor(), "Material has no specular color uniform");

		std::vector<UInt8>& bufferData = GetMaterial().GetUniformBufferData(m_uniformBlockIndex);
		float* colorPtr = AccessByOffset<float*>(bufferData.data(), m_pbrUniformOffsets.specularColor);
		colorPtr[0] = diffuse.r / 255.f;
		colorPtr[1] = diffuse.g / 255.f;
		colorPtr[2] = diffuse.b / 255.f;
		colorPtr[3] = diffuse.a / 255.f;
	}

	const std::shared_ptr<MaterialSettings>& PhysicallyBasedMaterial::GetSettings()
	{
		return s_pbrMaterialSettings;
	}

	MaterialSettings::Builder PhysicallyBasedMaterial::Build(PbrBuildOptions& options)
	{
		MaterialSettings::Builder settings = BasicMaterial::Build(options);

		assert(settings.uniformBlocks.size() == 1);
		std::vector<MaterialSettings::UniformVariable> variables = std::move(settings.uniformBlocks.front().uniforms);
		settings.uniformBlocks.clear();

		if (options.pbrOffsets.ambientColor != std::numeric_limits<std::size_t>::max())
		{
			variables.push_back({
				"AmbientColor",
				options.pbrOffsets.ambientColor
			});
		}

		if (options.pbrOffsets.shininess != std::numeric_limits<std::size_t>::max())
		{
			variables.push_back({
				"Shininess",
				options.pbrOffsets.shininess
			});
		}

		if (options.pbrOffsets.shininess != std::numeric_limits<std::size_t>::max())
		{
			variables.push_back({
				"SpecularColor",
				options.pbrOffsets.specularColor
			});
		}

		static_assert(sizeof(Vector4f) == 4 * sizeof(float), "Vector4f is expected to be exactly 4 floats wide");

		if (options.pbrOffsets.ambientColor != std::numeric_limits<std::size_t>::max())
			AccessByOffset<Vector4f&>(options.defaultValues.data(), options.pbrOffsets.ambientColor) = Vector4f(0.f, 0.f, 0.f, 1.f);

		if (options.pbrOffsets.specularColor != std::numeric_limits<std::size_t>::max())
			AccessByOffset<Vector4f&>(options.defaultValues.data(), options.pbrOffsets.specularColor) = Vector4f(1.f, 1.f, 1.f, 1.f);

		if (options.pbrOffsets.shininess != std::numeric_limits<std::size_t>::max())
			AccessByOffset<float&>(options.defaultValues.data(), options.pbrOffsets.shininess) = 2.f;

		// Textures
		if (options.pbrTextureIndexes)
			options.pbrTextureIndexes->emissive = settings.textures.size();

		settings.textures.push_back({
			7,
			"Emissive",
			ImageType::E2D
		});

		if (options.pbrTextureIndexes)
			options.pbrTextureIndexes->height = settings.textures.size();

		settings.textures.push_back({
			8,
			"Height",
			ImageType::E2D
		});

		if (options.pbrTextureIndexes)
			options.pbrTextureIndexes->metallic = settings.textures.size();
		
		settings.textures.push_back({
			9,
			"Metallic",
			ImageType::E2D
		});

		if (options.pbrTextureIndexes)
			options.pbrTextureIndexes->normal = settings.textures.size();

		settings.textures.push_back({
			10,
			"Normal",
			ImageType::E2D
		});

		if (options.pbrTextureIndexes)
			options.pbrTextureIndexes->roughness = settings.textures.size();
		
		settings.textures.push_back({
			11,
			"Roughness",
			ImageType::E2D
		});

		if (options.pbrTextureIndexes)
			options.pbrTextureIndexes->specular = settings.textures.size();

		settings.textures.push_back({
			12,
			"Specular",
			ImageType::E2D
		});

		if (options.uniformBlockIndex)
			*options.uniformBlockIndex = settings.uniformBlocks.size();

		settings.uniformBlocks.push_back({
			0,
			"MaterialSettings",
			options.pbrOffsets.totalSize,
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
		if (options.pbrOptionIndexes)
			options.pbrOptionIndexes->hasEmissiveMap = settings.options.size();

		MaterialSettings::BuildOption(settings.options, "HasEmissiveMap", "HasEmissiveTexture");

		// HasHeightMap
		if (options.pbrOptionIndexes)
			options.pbrOptionIndexes->hasHeightMap = settings.options.size();

		MaterialSettings::BuildOption(settings.options, "HasHeightMap", "HasHeightTexture");

		// HasNormalMap
		if (options.pbrOptionIndexes)
			options.pbrOptionIndexes->hasMetallicMap = settings.options.size();

		MaterialSettings::BuildOption(settings.options, "HasMetallicMap", "HasMetallicTexture");

		// HasNormalMap
		if (options.pbrOptionIndexes)
			options.pbrOptionIndexes->hasNormalMap = settings.options.size();

		MaterialSettings::BuildOption(settings.options, "HasNormalMap", "HasNormalTexture");

		// HasRoughnessMap
		if (options.pbrOptionIndexes)
			options.pbrOptionIndexes->hasRoughnessMap = settings.options.size();

		MaterialSettings::BuildOption(settings.options, "HasRoughnessMap", "HasRoughnessTexture");

		// HasSpecularMap
		if (options.pbrOptionIndexes)
			options.pbrOptionIndexes->hasSpecularMap = settings.options.size();

		MaterialSettings::BuildOption(settings.options, "HasSpecularMap", "HasSpecularTexture");

		return settings;
	}

	std::vector<std::shared_ptr<UberShader>> PhysicallyBasedMaterial::BuildShaders()
	{
		auto shader = std::make_shared<UberShader>(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, "PhysicallyBasedMaterial");

		return { std::move(shader) };
	}

	auto PhysicallyBasedMaterial::BuildUniformOffsets() -> std::pair<PbrUniformOffsets, nzsl::FieldOffsets>
	{
		auto basicOffsets = BasicMaterial::BuildUniformOffsets();
		nzsl::FieldOffsets fieldOffsets = basicOffsets.second;

		PbrUniformOffsets uniformOffsets;
		uniformOffsets.ambientColor = fieldOffsets.AddField(nzsl::StructFieldType::Float3);
		uniformOffsets.specularColor = fieldOffsets.AddField(nzsl::StructFieldType::Float3);
		uniformOffsets.shininess = fieldOffsets.AddField(nzsl::StructFieldType::Float1);

		uniformOffsets.totalSize = fieldOffsets.GetAlignedSize();

		return std::make_pair(std::move(uniformOffsets), std::move(fieldOffsets));
	}

	bool PhysicallyBasedMaterial::Initialize()
	{
		std::tie(s_pbrUniformOffsets, std::ignore) = BuildUniformOffsets();

		std::vector<UInt8> defaultValues(s_pbrUniformOffsets.totalSize);

		PbrBuildOptions options;
		options.defaultValues = std::move(defaultValues);
		options.shaders = BuildShaders();

		// Basic material
		options.basicOffsets = s_basicUniformOffsets;

		// Phong Material
		options.pbrOffsets = s_pbrUniformOffsets;
		options.pbrOptionIndexes = &s_pbrOptionIndexes;
		options.pbrTextureIndexes = &s_pbrTextureIndexes;

		s_pbrMaterialSettings = std::make_shared<MaterialSettings>(Build(options));

		return true;
	}

	void PhysicallyBasedMaterial::Uninitialize()
	{
		s_pbrMaterialSettings.reset();
	}

	std::shared_ptr<MaterialSettings> PhysicallyBasedMaterial::s_pbrMaterialSettings;
	std::size_t PhysicallyBasedMaterial::s_pbrUniformBlockIndex;
	PhysicallyBasedMaterial::PbrOptionIndexes PhysicallyBasedMaterial::s_pbrOptionIndexes;
	PhysicallyBasedMaterial::PbrTextureIndexes PhysicallyBasedMaterial::s_pbrTextureIndexes;
	PhysicallyBasedMaterial::PbrUniformOffsets PhysicallyBasedMaterial::s_pbrUniformOffsets;
}
