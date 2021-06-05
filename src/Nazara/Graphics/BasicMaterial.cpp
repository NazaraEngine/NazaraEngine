// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Graphics/UberShader.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Shader/Ast/AstSerializer.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/FieldOffsets.hpp>
#include <Nazara/Utility/MaterialData.hpp>
#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace
	{
		const UInt8 r_fragmentShader[] = {
			#include <Nazara/Graphics/Resources/Shaders/basicmaterial.frag.shader.h>
		};

		const UInt8 r_vertexShader[] = {
			#include <Nazara/Graphics/Resources/Shaders/basicmaterial.vert.shader.h>
		};
	}

	BasicMaterial::BasicMaterial(Material& material) :
	m_material(material)
	{
		// Most common case: don't fetch texture indexes as a little optimization
		const std::shared_ptr<const MaterialSettings>& materialSettings = material.GetSettings();
		if (materialSettings == s_materialSettings)
		{
			m_conditionIndexes = s_conditionIndexes;
			m_textureIndexes = s_textureIndexes;
			m_uniformBlockIndex = s_uniformBlockIndex;
			m_uniformOffsets = s_uniformOffsets;
		}
		else
		{
			m_conditionIndexes.alphaTest = materialSettings->GetConditionIndex("AlphaTest");
			m_conditionIndexes.hasAlphaMap = materialSettings->GetConditionIndex("HasAlphaMap");
			m_conditionIndexes.hasDiffuseMap = materialSettings->GetConditionIndex("HasDiffuseMap");

			m_textureIndexes.alpha = materialSettings->GetTextureIndex("Alpha");
			m_textureIndexes.diffuse = materialSettings->GetTextureIndex("Diffuse");

			m_uniformBlockIndex = materialSettings->GetUniformBlockIndex("BasicSettings");

			m_uniformOffsets.alphaThreshold = materialSettings->GetUniformBlockVariableOffset(m_uniformBlockIndex, "AlphaThreshold");
			m_uniformOffsets.diffuseColor = materialSettings->GetUniformBlockVariableOffset(m_uniformBlockIndex, "DiffuseColor");
		}
	}

	float BasicMaterial::GetAlphaTestThreshold() const
	{
		NazaraAssert(HasAlphaTestThreshold(), "Material has no alpha threshold uniform");

		const std::vector<UInt8>& bufferData = m_material.GetUniformBufferConstData(m_uniformBlockIndex);

		return AccessByOffset<const float&>(bufferData.data(), m_uniformOffsets.alphaThreshold);
	}

	Color BasicMaterial::GetDiffuseColor() const
	{
		NazaraAssert(HasDiffuseColor(), "Material has no diffuse color uniform");

		const std::vector<UInt8>& bufferData = m_material.GetUniformBufferConstData(m_uniformBlockIndex);

		const float* colorPtr = AccessByOffset<const float*>(bufferData.data(), m_uniformOffsets.diffuseColor);
		return Color(colorPtr[0] * 255, colorPtr[1] * 255, colorPtr[2] * 255, colorPtr[3] * 255); //< TODO: Make color able to use float
	}

	void BasicMaterial::SetAlphaTestThreshold(float alphaThreshold)
	{
		NazaraAssert(HasAlphaTestThreshold(), "Material has no alpha threshold uniform");

		std::vector<UInt8>& bufferData = m_material.GetUniformBufferData(m_uniformBlockIndex);
		AccessByOffset<float&>(bufferData.data(), m_uniformOffsets.alphaThreshold) = alphaThreshold;
	}

	void BasicMaterial::SetDiffuseColor(const Color& diffuse)
	{
		NazaraAssert(HasDiffuseColor(), "Material has no diffuse color uniform");

		std::vector<UInt8>& bufferData = m_material.GetUniformBufferData(m_uniformBlockIndex);

		float* colorPtr = AccessByOffset<float*>(bufferData.data(), m_uniformOffsets.diffuseColor);
		colorPtr[0] = diffuse.r / 255.f;
		colorPtr[1] = diffuse.g / 255.f;
		colorPtr[2] = diffuse.b / 255.f;
		colorPtr[3] = diffuse.a / 255.f;
	}

	bool BasicMaterial::Initialize()
	{
		FieldOffsets fieldOffsets(StructLayout::Std140);

		s_uniformOffsets.alphaThreshold = fieldOffsets.AddField(StructFieldType::Float1);
		s_uniformOffsets.diffuseColor = fieldOffsets.AddField(StructFieldType::Float4);
		s_uniformOffsets.totalSize = fieldOffsets.GetSize();

		MaterialSettings::Builder settings;
		settings.predefinedBinding.fill(MaterialSettings::InvalidIndex);

		std::vector<MaterialSettings::UniformVariable> variables;
		variables.assign({
			{
				"AlphaThreshold",
				s_uniformOffsets.alphaThreshold
			},
			{
				"DiffuseColor",
				s_uniformOffsets.diffuseColor
			}
		});

		static_assert(sizeof(Vector4f) == 4 * sizeof(float), "Vector4f is expected to be exactly 4 floats wide");

		std::vector<UInt8> defaultValues(fieldOffsets.GetSize());
		AccessByOffset<Vector4f&>(defaultValues.data(), s_uniformOffsets.diffuseColor) = Vector4f(1.f, 1.f, 1.f, 1.f);
		AccessByOffset<float&>(defaultValues.data(), s_uniformOffsets.alphaThreshold) = 0.2f;
		
		s_textureIndexes.alpha = settings.textures.size();
		settings.textures.push_back({
			"MaterialAlphaMap",
			"Alpha",
			ImageType::E2D
		});

		s_textureIndexes.diffuse = settings.textures.size();
		settings.textures.push_back({
			"MaterialDiffuseMap",
			"Diffuse",
			ImageType::E2D
		});

		settings.predefinedBinding[UnderlyingCast(PredefinedShaderBinding::TexOverlay)] = settings.textures.size();
		settings.textures.push_back({
			"TextureOverlay",
			"Overlay",
			ImageType::E2D
		});

		s_uniformBlockIndex = settings.uniformBlocks.size();
		settings.uniformBlocks.assign({
			{
				fieldOffsets.GetSize(),
				"BasicSettings",
				"MaterialBasicSettings",
				std::move(variables),
				std::move(defaultValues)
			}
		});

		settings.predefinedBinding[UnderlyingCast(PredefinedShaderBinding::UboInstanceData)] = settings.textures.size() + settings.uniformBlocks.size() + settings.sharedUniformBlocks.size();
		settings.sharedUniformBlocks.push_back(PredefinedInstanceData::GetUniformBlock());

		settings.predefinedBinding[UnderlyingCast(PredefinedShaderBinding::UboViewerData)] = settings.textures.size() + settings.uniformBlocks.size() + settings.sharedUniformBlocks.size();
		settings.sharedUniformBlocks.push_back(PredefinedViewerData::GetUniformBlock());

		// Shaders
		auto& fragmentShader = settings.shaders[UnderlyingCast(ShaderStageType::Fragment)];
		auto& vertexShader = settings.shaders[UnderlyingCast(ShaderStageType::Vertex)];

		fragmentShader = std::make_shared<UberShader>(ShaderStageType::Fragment, ShaderAst::UnserializeShader(r_fragmentShader, sizeof(r_fragmentShader)));
		vertexShader = std::make_shared<UberShader>(ShaderStageType::Vertex, ShaderAst::UnserializeShader(r_vertexShader, sizeof(r_vertexShader)));

		// Conditions

		// HasDiffuseMap
		{
			std::array<UInt64, ShaderStageTypeCount> shaderConditions;
			shaderConditions.fill(0);
			shaderConditions[UnderlyingCast(ShaderStageType::Fragment)] = fragmentShader->GetOptionFlagByName("HAS_DIFFUSE_TEXTURE");

			s_conditionIndexes.hasDiffuseMap = settings.conditions.size();
			settings.conditions.push_back({
				"HasDiffuseMap",
				shaderConditions
			});
		}

		// HasAlphaMap
		{
			std::array<UInt64, ShaderStageTypeCount> shaderConditions;
			shaderConditions.fill(0);
			shaderConditions[UnderlyingCast(ShaderStageType::Fragment)] = fragmentShader->GetOptionFlagByName("HAS_ALPHA_TEXTURE");

			s_conditionIndexes.hasAlphaMap = settings.conditions.size();
			settings.conditions.push_back({
				"HasAlphaMap",
				shaderConditions
			});
		}

		// AlphaTest
		{
			std::array<UInt64, ShaderStageTypeCount> shaderConditions;
			shaderConditions.fill(0);
			shaderConditions[UnderlyingCast(ShaderStageType::Fragment)] = fragmentShader->GetOptionFlagByName("ALPHA_TEST");

			s_conditionIndexes.alphaTest = settings.conditions.size();
			settings.conditions.push_back({
				"AlphaTest",
				shaderConditions
			});
		}

		s_materialSettings = std::make_shared<MaterialSettings>(std::move(settings));

		return true;
	}

	void BasicMaterial::Uninitialize()
	{
		s_materialSettings.reset();
	}

	std::shared_ptr<MaterialSettings> BasicMaterial::s_materialSettings;
	std::size_t BasicMaterial::s_uniformBlockIndex;
	BasicMaterial::ConditionIndexes BasicMaterial::s_conditionIndexes;
	BasicMaterial::TextureIndexes BasicMaterial::s_textureIndexes;
	BasicMaterial::UniformOffsets BasicMaterial::s_uniformOffsets;
}
