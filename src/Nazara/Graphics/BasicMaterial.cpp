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
			#include <Nazara/Graphics/Resources/Shaders/basic_material.nzsl.h>
		};
	}

	BasicMaterial::BasicMaterial(MaterialPass& material) :
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

	MaterialSettings::Builder BasicMaterial::Build(const UniformOffsets& offsets, std::vector<UInt8> defaultValues, std::vector<std::shared_ptr<UberShader>> uberShaders, std::size_t* uniformBlockIndex, OptionIndexes* optionIndexes, TextureIndexes* textureIndexes)
	{
		MaterialSettings::Builder settings;

		std::vector<MaterialSettings::UniformVariable> variables;
		if (offsets.alphaThreshold != std::numeric_limits<std::size_t>::max())
		{
			variables.push_back({
				"AlphaThreshold",
				offsets.alphaThreshold
			});
		}

		if (offsets.diffuseColor != std::numeric_limits<std::size_t>::max())
		{
			variables.push_back({
				"DiffuseColor",
				offsets.diffuseColor
			});
		}

		if (offsets.alphaThreshold != std::numeric_limits<std::size_t>::max())
			AccessByOffset<float&>(defaultValues.data(), offsets.alphaThreshold) = 0.2f;

		static_assert(sizeof(Vector4f) == 4 * sizeof(float), "Vector4f is expected to be exactly 4 floats wide");
		if (offsets.diffuseColor != std::numeric_limits<std::size_t>::max())
			AccessByOffset<Vector4f&>(defaultValues.data(), offsets.diffuseColor) = Vector4f(1.f, 1.f, 1.f, 1.f);

		// Textures
		if (textureIndexes)
			textureIndexes->alpha = settings.textures.size();

		settings.textures.push_back({
			2,
			"Alpha",
			ImageType::E2D
		});

		if (textureIndexes)
			textureIndexes->diffuse = settings.textures.size();

		settings.textures.push_back({
			1,
			"Diffuse",
			ImageType::E2D
		});

		if (uniformBlockIndex)
			*uniformBlockIndex = settings.uniformBlocks.size();

		settings.uniformBlocks.push_back({
			0,
			"BasicSettings",
			offsets.totalSize,
			std::move(variables),
			std::move(defaultValues)
		});

		settings.shaders = std::move(uberShaders);

		// Options

		// HasDiffuseMap
		if (optionIndexes)
			optionIndexes->hasDiffuseMap = settings.options.size();

		MaterialSettings::BuildOption(settings.options, settings.shaders, "HasDiffuseMap", "HAS_DIFFUSE_TEXTURE");

		// HasAlphaMap
		if (optionIndexes)
			optionIndexes->hasAlphaMap = settings.options.size();

		MaterialSettings::BuildOption(settings.options, settings.shaders, "HasAlphaMap", "HAS_ALPHA_TEXTURE");

		// AlphaTest
		if (optionIndexes)
			optionIndexes->alphaTest = settings.options.size();

		MaterialSettings::BuildOption(settings.options, settings.shaders, "AlphaTest", "ALPHA_TEST");

		return settings;
	}

	std::vector<std::shared_ptr<UberShader>> BasicMaterial::BuildShaders()
	{
		ShaderAst::StatementPtr shaderAst = ShaderLang::Parse(std::string_view(reinterpret_cast<const char*>(r_shader), sizeof(r_shader)));
		auto shader = std::make_shared<UberShader>(ShaderStageType::Fragment | ShaderStageType::Vertex, shaderAst);

		return { std::move(shader) };
	}

	auto BasicMaterial::BuildUniformOffsets() -> std::pair<UniformOffsets, FieldOffsets>
	{
		FieldOffsets fieldOffsets(StructLayout::Std140);

		UniformOffsets uniformOffsets;
		uniformOffsets.alphaThreshold = fieldOffsets.AddField(StructFieldType::Float1);
		uniformOffsets.diffuseColor = fieldOffsets.AddField(StructFieldType::Float4);
		uniformOffsets.totalSize = fieldOffsets.GetSize();

		return std::make_pair(std::move(uniformOffsets), std::move(fieldOffsets));
	}

	bool BasicMaterial::Initialize()
	{
		std::tie(s_uniformOffsets, std::ignore) = BuildUniformOffsets();

		std::vector<UInt8> defaultValues(s_uniformOffsets.totalSize);
		s_materialSettings = std::make_shared<MaterialSettings>(Build(s_uniformOffsets, std::move(defaultValues), BuildShaders(), &s_uniformBlockIndex, &s_optionIndexes, &s_textureIndexes));

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
