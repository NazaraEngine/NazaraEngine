// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Graphics/UberShader.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Shader/ShaderLangLexer.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
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
		const UInt8 r_shader[] = {
			#include <Nazara/Graphics/Resources/Shaders/basicmaterial.nzsl.h>
		};
	}

	BasicMaterial::BasicMaterial(Material& material) :
	m_material(material)
	{
		// Most common case: don't fetch texture indexes as a little optimization
		const std::shared_ptr<const MaterialSettings>& materialSettings = material.GetSettings();
		if (materialSettings == s_materialSettings)
		{
			m_optionIndexes = s_optionIndexes;
			m_textureIndexes = s_textureIndexes;
			m_uniformBlockIndex = s_uniformBlockIndex;
			m_uniformOffsets = s_uniformOffsets;
		}
		else
		{
			m_optionIndexes.alphaTest = materialSettings->GetOptionIndex("AlphaTest");
			m_optionIndexes.hasAlphaMap = materialSettings->GetOptionIndex("HasAlphaMap");
			m_optionIndexes.hasDiffuseMap = materialSettings->GetOptionIndex("HasDiffuseMap");

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

		s_uniformBlockIndex = settings.uniformBlocks.size();
		settings.uniformBlocks.push_back({
			0,
			"BasicSettings",
			fieldOffsets.GetSize(),
			std::move(variables),
			std::move(defaultValues)
		});

		// Shaders
		ShaderAst::StatementPtr shaderAst = ShaderLang::Parse(std::string_view(reinterpret_cast<const char*>(r_shader), sizeof(r_shader)));
		auto uberShader = std::make_shared<UberShader>(ShaderStageType::Fragment | ShaderStageType::Vertex, shaderAst);

		settings.shaders.emplace_back(uberShader);

		// Options

		// HasDiffuseMap
		{
			std::array<UInt64, ShaderStageTypeCount> shaderOptions;
			shaderOptions.fill(0);
			shaderOptions[UnderlyingCast(ShaderStageType::Fragment)] = uberShader->GetOptionFlagByName("HAS_DIFFUSE_TEXTURE");
			shaderOptions[UnderlyingCast(ShaderStageType::Vertex)] = uberShader->GetOptionFlagByName("HAS_DIFFUSE_TEXTURE");

			s_optionIndexes.hasDiffuseMap = settings.options.size();
			settings.options.push_back({
				"HasDiffuseMap",
				shaderOptions
			});
		}

		// HasAlphaMap
		{
			std::array<UInt64, ShaderStageTypeCount> shaderOptions;
			shaderOptions.fill(0);
			shaderOptions[UnderlyingCast(ShaderStageType::Fragment)] = uberShader->GetOptionFlagByName("HAS_ALPHA_TEXTURE");
			shaderOptions[UnderlyingCast(ShaderStageType::Vertex)] = uberShader->GetOptionFlagByName("HAS_ALPHA_TEXTURE");

			s_optionIndexes.hasAlphaMap = settings.options.size();
			settings.options.push_back({
				"HasAlphaMap",
				shaderOptions
			});
		}

		// AlphaTest
		{
			std::array<UInt64, ShaderStageTypeCount> shaderOptions;
			shaderOptions.fill(0);
			shaderOptions[UnderlyingCast(ShaderStageType::Fragment)] = uberShader->GetOptionFlagByName("ALPHA_TEST");

			s_optionIndexes.alphaTest = settings.options.size();
			settings.options.push_back({
				"AlphaTest",
				shaderOptions
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
	BasicMaterial::OptionIndexes BasicMaterial::s_optionIndexes;
	BasicMaterial::TextureIndexes BasicMaterial::s_textureIndexes;
	BasicMaterial::UniformOffsets BasicMaterial::s_uniformOffsets;
}
