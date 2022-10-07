// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <NZSL/Math/FieldOffsets.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	BasicMaterial::BasicMaterial(Material& material) :
	m_isDepthPassEnabled(true),
	m_isForwardPassEnabled(true)
	{
		m_depthPass = material.FindPass("DepthPass");
		m_forwardPass = material.FindPass("ForwardPass");
	}

	std::shared_ptr<Material> BasicMaterial::Build(const BasicConfig& config)
	{
		auto material = std::make_shared<Material>();

		return std::shared_ptr<Material>();
	}

	MaterialSettings::Builder BasicMaterial::BuildSettings(const BasicBuildSettings& options)
	{
		MaterialSettings::Builder settings;

		std::vector<MaterialSettings::UniformVariable> variables;
		if (options.basicUniformOffsets->alphaThreshold != std::numeric_limits<std::size_t>::max())
		{
			variables.push_back({
				"AlphaThreshold",
				options.basicUniformOffsets->alphaThreshold
			});
		}

		if (options.basicUniformOffsets->baseColor != std::numeric_limits<std::size_t>::max())
		{
			variables.push_back({
				"BaseColor",
				options.basicUniformOffsets->baseColor
			});
		}

		static_assert(sizeof(Vector4f) == 4 * sizeof(float), "Vector4f is expected to be exactly 4 floats wide");

		if (options.basicUniformOffsets->alphaThreshold != std::numeric_limits<std::size_t>::max())
			AccessByOffset<float&>(options.defaultValues.data(), options.basicUniformOffsets->alphaThreshold) = 0.2f;

		if (options.basicUniformOffsets->baseColor != std::numeric_limits<std::size_t>::max())
			AccessByOffset<Vector4f&>(options.defaultValues.data(), options.basicUniformOffsets->baseColor) = Vector4f(1.f, 1.f, 1.f, 1.f);

		// Textures
		if (options.basicTextureIndexes)
			options.basicTextureIndexes->alpha = settings.textures.size();

		settings.textures.push_back({
			2,
			"Alpha",
			ImageType::E2D
		});

		if (options.basicTextureIndexes)
			options.basicTextureIndexes->baseColor = settings.textures.size();

		settings.textures.push_back({
			1,
			"BaseColor",
			ImageType::E2D
		});

		if (options.uniformBlockIndex)
			*options.uniformBlockIndex = settings.uniformBlocks.size();

		settings.uniformBlocks.push_back({
			0,
			"MaterialSettings",
			options.basicUniformOffsets->totalSize,
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
		settings.sharedUniformBlocks.push_back(PredefinedSkeletalData::GetUniformBlock(6, nzsl::ShaderStageType::Vertex));
		settings.sharedUniformBlocks.push_back(PredefinedViewerData::GetUniformBlock(5, nzsl::ShaderStageType_All));

		settings.predefinedBindings[UnderlyingCast(PredefinedShaderBinding::InstanceDataUbo)] = 4;
		settings.predefinedBindings[UnderlyingCast(PredefinedShaderBinding::OverlayTexture)] = 3;
		settings.predefinedBindings[UnderlyingCast(PredefinedShaderBinding::SkeletalDataUbo)] = 6;
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

						case VertexComponent::JointIndices:
							config.optionValues[CRC32("JointIndicesLocation")] = locationIndex;
							break;

						case VertexComponent::JointWeights:
							config.optionValues[CRC32("JointWeightsLocation")] = locationIndex;
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

		// HasBaseColorMap
		if (options.basicOptionIndexes)
			options.basicOptionIndexes->hasBaseColorMap = settings.options.size();

		MaterialSettings::BuildOption(settings.options, "HasBaseColorMap", "HasBaseColorTexture");

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

	std::vector<std::shared_ptr<UberShader>> BasicMaterial::BuildDepthPassShaders()
	{
		auto shader = std::make_shared<UberShader>(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, "DepthMaterial");

		return { std::move(shader) };
	}

	std::vector<std::shared_ptr<UberShader>> BasicMaterial::BuildForwardPassShaders()
	{
		auto shader = std::make_shared<UberShader>(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, "BasicMaterialPass");

		return { std::move(shader) };
	}

	auto BasicMaterial::BuildUniformOffsets() -> std::pair<BasicUniformOffsets, nzsl::FieldOffsets>
	{
		nzsl::FieldOffsets fieldOffsets(nzsl::StructLayout::Std140);

		BasicUniformOffsets uniformOffsets;
		uniformOffsets.alphaThreshold = fieldOffsets.AddField(nzsl::StructFieldType::Float1);
		uniformOffsets.baseColor = fieldOffsets.AddField(nzsl::StructFieldType::Float4);
		uniformOffsets.totalSize = fieldOffsets.GetAlignedSize();

		return std::make_pair(std::move(uniformOffsets), std::move(fieldOffsets));
	}

	bool BasicMaterial::Initialize()
	{
		std::tie(s_basicUniformOffsets, std::ignore) = BuildUniformOffsets();

		std::vector<UInt8> defaultValues(s_basicUniformOffsets.totalSize);

		BasicBuildSettings settings;
		settings.defaultValues.resize(s_basicUniformOffsets.totalSize);
		settings.shaders = BuildForwardPassShaders();

		settings.basicOptionIndexes = &s_basicOptionIndexes;
		settings.basicTextureIndexes = &s_basicTextureIndexes;
		settings.basicUniformOffsets = &s_basicUniformOffsets;
		settings.uniformBlockIndex = &s_uniformBlockIndex;

		// Forward pass
		s_basicMaterialSettings = std::make_shared<MaterialSettings>(BuildSettings(settings));

		// Depth pass
		settings.shaders = BuildDepthPassShaders();

		s_basicMaterialSettings = std::make_shared<MaterialSettings>(BuildSettings(settings));

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
