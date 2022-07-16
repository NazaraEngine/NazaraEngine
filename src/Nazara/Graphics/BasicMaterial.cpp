// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Graphics/UberShader.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/MaterialData.hpp>
#include <NZSL/Parser.hpp>
#include <NZSL/Math/FieldOffsets.hpp>
#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	BasicMaterial::BasicMaterial(MaterialPass& material) :
	BasicMaterial(material, NoInit{})
	{
		// Most common case: don't fetch texture indexes as a little optimization
		const std::shared_ptr<const MaterialSettings>& materialSettings = material.GetSettings();
		if (materialSettings == s_basicMaterialSettings)
		{
			m_basicOptionIndexes = s_basicOptionIndexes;
			m_basicTextureIndexes = s_basicTextureIndexes;
			m_uniformBlockIndex = s_uniformBlockIndex;
			m_basicUniformOffsets = s_basicUniformOffsets;
		}
		else
		{
			m_basicOptionIndexes.alphaTest = materialSettings->GetOptionIndex("AlphaTest");
			m_basicOptionIndexes.hasAlphaMap = materialSettings->GetOptionIndex("HasAlphaMap");
			m_basicOptionIndexes.hasDiffuseMap = materialSettings->GetOptionIndex("HasDiffuseMap");

			m_basicTextureIndexes.alpha = materialSettings->GetTextureIndex("Alpha");
			m_basicTextureIndexes.diffuse = materialSettings->GetTextureIndex("Diffuse");

			m_uniformBlockIndex = materialSettings->GetUniformBlockIndex("MaterialSettings");
			if (m_uniformBlockIndex != MaterialSettings::InvalidIndex)
			{
				m_basicUniformOffsets.alphaThreshold = materialSettings->GetUniformBlockVariableOffset(m_uniformBlockIndex, "AlphaThreshold");
				m_basicUniformOffsets.diffuseColor = materialSettings->GetUniformBlockVariableOffset(m_uniformBlockIndex, "DiffuseColor");
			}
			else
			{
				m_basicUniformOffsets.alphaThreshold = MaterialSettings::InvalidIndex;
				m_basicUniformOffsets.diffuseColor = MaterialSettings::InvalidIndex;
			}
		}
	}

	float BasicMaterial::GetAlphaTestThreshold() const
	{
		NazaraAssert(HasAlphaTestThreshold(), "Material has no alpha threshold uniform");

		const std::vector<UInt8>& bufferData = m_material.GetUniformBufferConstData(m_uniformBlockIndex);

		return AccessByOffset<const float&>(bufferData.data(), m_basicUniformOffsets.alphaThreshold);
	}

	Color BasicMaterial::GetDiffuseColor() const
	{
		NazaraAssert(HasDiffuseColor(), "Material has no diffuse color uniform");

		const std::vector<UInt8>& bufferData = m_material.GetUniformBufferConstData(m_uniformBlockIndex);

		const float* colorPtr = AccessByOffset<const float*>(bufferData.data(), m_basicUniformOffsets.diffuseColor);
		return Color(colorPtr[0], colorPtr[1], colorPtr[2], colorPtr[3]);
	}

	void BasicMaterial::SetAlphaTestThreshold(float alphaThreshold)
	{
		NazaraAssert(HasAlphaTestThreshold(), "Material has no alpha threshold uniform");

		std::vector<UInt8>& bufferData = m_material.GetUniformBufferData(m_uniformBlockIndex);
		AccessByOffset<float&>(bufferData.data(), m_basicUniformOffsets.alphaThreshold) = alphaThreshold;
	}

	void BasicMaterial::SetDiffuseColor(const Color& diffuse)
	{
		NazaraAssert(HasDiffuseColor(), "Material has no diffuse color uniform");

		std::vector<UInt8>& bufferData = m_material.GetUniformBufferData(m_uniformBlockIndex);

		float* colorPtr = AccessByOffset<float*>(bufferData.data(), m_basicUniformOffsets.diffuseColor);
		colorPtr[0] = diffuse.r;
		colorPtr[1] = diffuse.g;
		colorPtr[2] = diffuse.b;
		colorPtr[3] = diffuse.a;
	}

	MaterialSettings::Builder BasicMaterial::Build(BasicBuildOptions& options)
	{
		MaterialSettings::Builder settings;

		std::vector<MaterialSettings::UniformVariable> variables;
		if (options.basicOffsets.alphaThreshold != std::numeric_limits<std::size_t>::max())
		{
			variables.push_back({
				"AlphaThreshold",
				options.basicOffsets.alphaThreshold
			});
		}

		if (options.basicOffsets.diffuseColor != std::numeric_limits<std::size_t>::max())
		{
			variables.push_back({
				"DiffuseColor",
				options.basicOffsets.diffuseColor
			});
		}

		static_assert(sizeof(Vector4f) == 4 * sizeof(float), "Vector4f is expected to be exactly 4 floats wide");

		if (options.basicOffsets.alphaThreshold != std::numeric_limits<std::size_t>::max())
			AccessByOffset<float&>(options.defaultValues.data(), options.basicOffsets.alphaThreshold) = 0.2f;

		if (options.basicOffsets.diffuseColor != std::numeric_limits<std::size_t>::max())
			AccessByOffset<Vector4f&>(options.defaultValues.data(), options.basicOffsets.diffuseColor) = Vector4f(1.f, 1.f, 1.f, 1.f);

		// Textures
		if (options.basicTextureIndexes)
			options.basicTextureIndexes->alpha = settings.textures.size();

		settings.textures.push_back({
			2,
			"Alpha",
			ImageType::E2D
		});

		if (options.basicTextureIndexes)
			options.basicTextureIndexes->diffuse = settings.textures.size();

		settings.textures.push_back({
			1,
			"Diffuse",
			ImageType::E2D
		});

		if (options.uniformBlockIndex)
			*options.uniformBlockIndex = settings.uniformBlocks.size();

		settings.uniformBlocks.push_back({
			0,
			"MaterialSettings",
			options.basicOffsets.totalSize,
			std::move(variables),
			options.defaultValues
		});

		// Common data
		settings.textures.push_back({
			3,
			"TextureOverlay",
			ImageType::E2D
		});

		settings.sharedUniformBlocks.push_back(PredefinedInstanceData::GetUniformBlock(4, nzsl::ShaderStageType::Vertex));
		settings.sharedUniformBlocks.push_back(PredefinedViewerData::GetUniformBlock(5, nzsl::ShaderStageType_All));

		settings.predefinedBindings[UnderlyingCast(PredefinedShaderBinding::InstanceDataUbo)] = 4;
		settings.predefinedBindings[UnderlyingCast(PredefinedShaderBinding::OverlayTexture)] = 3;
		settings.predefinedBindings[UnderlyingCast(PredefinedShaderBinding::ViewerDataUbo)] = 5;

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

		// HasDiffuseMap
		if (options.basicOptionIndexes)
			options.basicOptionIndexes->hasDiffuseMap = settings.options.size();

		MaterialSettings::BuildOption(settings.options, "HasDiffuseMap", "HasDiffuseTexture");

		// HasAlphaMap
		if (options.basicOptionIndexes)
			options.basicOptionIndexes->hasAlphaMap = settings.options.size();

		MaterialSettings::BuildOption(settings.options, "HasAlphaMap", "HasAlphaTexture");

		// AlphaTest
		if (options.basicOptionIndexes)
			options.basicOptionIndexes->alphaTest = settings.options.size();

		MaterialSettings::BuildOption(settings.options, "AlphaTest", "AlphaTest");

		return settings;
	}

	std::vector<std::shared_ptr<UberShader>> BasicMaterial::BuildShaders()
	{
		auto shader = std::make_shared<UberShader>(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, "BasicMaterial");

		return { std::move(shader) };
	}

	auto BasicMaterial::BuildUniformOffsets() -> std::pair<BasicUniformOffsets, nzsl::FieldOffsets>
	{
		nzsl::FieldOffsets fieldOffsets(nzsl::StructLayout::Std140);

		BasicUniformOffsets uniformOffsets;
		uniformOffsets.alphaThreshold = fieldOffsets.AddField(nzsl::StructFieldType::Float1);
		uniformOffsets.diffuseColor = fieldOffsets.AddField(nzsl::StructFieldType::Float4);
		uniformOffsets.totalSize = fieldOffsets.GetAlignedSize();

		return std::make_pair(std::move(uniformOffsets), std::move(fieldOffsets));
	}

	bool BasicMaterial::Initialize()
	{
		std::tie(s_basicUniformOffsets, std::ignore) = BuildUniformOffsets();

		std::vector<UInt8> defaultValues(s_basicUniformOffsets.totalSize);

		BasicBuildOptions options;
		options.defaultValues.resize(s_basicUniformOffsets.totalSize);
		options.shaders = BuildShaders();

		options.basicOffsets = s_basicUniformOffsets;
		options.basicOptionIndexes = &s_basicOptionIndexes;
		options.basicTextureIndexes = &s_basicTextureIndexes;
		options.uniformBlockIndex = &s_uniformBlockIndex;

		s_basicMaterialSettings = std::make_shared<MaterialSettings>(Build(options));

		return true;
	}

	void BasicMaterial::Uninitialize()
	{
		s_basicMaterialSettings.reset();
	}

	std::shared_ptr<MaterialSettings> BasicMaterial::s_basicMaterialSettings;
	std::size_t BasicMaterial::s_uniformBlockIndex;
	BasicMaterial::BasicOptionIndexes BasicMaterial::s_basicOptionIndexes;
	BasicMaterial::BasicTextureIndexes BasicMaterial::s_basicTextureIndexes;
	BasicMaterial::BasicUniformOffsets BasicMaterial::s_basicUniformOffsets;
}
