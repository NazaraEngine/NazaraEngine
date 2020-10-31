// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/BasicMaterial.hpp>
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
			m_textureIndexes = s_textureIndexes;
			m_uniformBlockIndex = s_uniformBlockIndex;
			m_uniformOffsets = s_uniformOffsets;
		}
		else
		{
			m_textureIndexes.alpha = materialSettings->GetTextureIndex("Alpha");
			m_textureIndexes.diffuse = materialSettings->GetTextureIndex("Diffuse");

			m_uniformBlockIndex = materialSettings->GetUniformBlockIndex("BasicSettings");

			m_uniformOffsets.alphaThreshold = materialSettings->GetUniformBlockVariableOffset(m_uniformBlockIndex, "AlphaThreshold");
			m_uniformOffsets.diffuseColor = materialSettings->GetUniformBlockVariableOffset(m_uniformBlockIndex, "DiffuseColor");
		}
	}

	float BasicMaterial::GetAlphaThreshold() const
	{
		NazaraAssert(HasAlphaThreshold(), "Material has no alpha threshold uniform");

		BufferMapper<UniformBuffer> mapper(m_material.GetUniformBuffer(m_uniformBlockIndex), BufferAccess_ReadOnly);
		return AccessByOffset<const float&>(mapper.GetPointer(), m_uniformOffsets.alphaThreshold);
	}

	Color BasicMaterial::GetDiffuseColor() const
	{
		NazaraAssert(HasDiffuseColor(), "Material has no diffuse color uniform");

		BufferMapper<UniformBuffer> mapper(m_material.GetUniformBuffer(m_uniformBlockIndex), BufferAccess_ReadOnly);

		const float* colorPtr = AccessByOffset<const float*>(mapper.GetPointer(), m_uniformOffsets.diffuseColor);
		return Color(colorPtr[0] * 255, colorPtr[1] * 255, colorPtr[2] * 255, colorPtr[3] * 255); //< TODO: Make color able to use float
	}

	void BasicMaterial::SetAlphaThreshold(float alphaThreshold)
	{
		NazaraAssert(HasAlphaThreshold(), "Material has no alpha threshold uniform");

		BufferMapper<UniformBuffer> mapper(m_material.GetUniformBuffer(m_uniformBlockIndex), BufferAccess_WriteOnly);
		AccessByOffset<float&>(mapper.GetPointer(), m_uniformOffsets.alphaThreshold) = alphaThreshold;
	}

	void BasicMaterial::SetDiffuseColor(const Color& diffuse)
	{
		NazaraAssert(HasDiffuseColor(), "Material has no diffuse color uniform");

		BufferMapper<UniformBuffer> mapper(m_material.GetUniformBuffer(m_uniformBlockIndex), BufferAccess_WriteOnly);
		float* colorPtr = AccessByOffset<float*>(mapper.GetPointer(), m_uniformOffsets.diffuseColor);
		colorPtr[0] = diffuse.r / 255.f;
		colorPtr[1] = diffuse.g / 255.f;
		colorPtr[2] = diffuse.b / 255.f;
		colorPtr[3] = diffuse.a / 255.f;
	}

	const std::shared_ptr<MaterialSettings>& BasicMaterial::GetSettings()
	{
		return s_materialSettings;
	}

	bool BasicMaterial::Initialize()
	{
		FieldOffsets fieldOffsets(StructLayout_Std140);

		s_uniformOffsets.diffuseColor = fieldOffsets.AddField(StructFieldType_Float4);
		s_uniformOffsets.alphaThreshold = fieldOffsets.AddField(StructFieldType_Float1);

		MaterialSettings::PredefinedBinding predefinedBinding;
		predefinedBinding.fill(MaterialSettings::InvalidIndex);

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
		
		std::vector<MaterialSettings::Texture> textures;
		s_textureIndexes.alpha = textures.size();
		textures.push_back({
			"MaterialAlphaMap",
			"Alpha",
			ImageType_2D
		});
		
		s_textureIndexes.diffuse = textures.size();
		textures.push_back({
			"MaterialDiffuseMap",
			"Diffuse",
			ImageType_2D
		});

		predefinedBinding[UnderlyingCast(PredefinedShaderBinding::TexOverlay)] = textures.size();
		textures.push_back({
			"TextureOverlay",
			"Overlay",
			ImageType_2D
		});

		std::vector<MaterialSettings::UniformBlock> uniformBlocks;
		s_uniformBlockIndex = uniformBlocks.size();
		uniformBlocks.assign({
			{
				fieldOffsets.GetSize(),
				"BasicSettings",
				"MaterialBasicSettings",
				std::move(variables),
				std::move(defaultValues)
			}
		});

		std::vector<MaterialSettings::SharedUniformBlock> sharedUniformBlock;

		predefinedBinding[UnderlyingCast(PredefinedShaderBinding::UboInstanceData)] = textures.size() + uniformBlocks.size() + sharedUniformBlock.size();
		sharedUniformBlock.push_back(PredefinedInstanceData::GetUniformBlock());
		predefinedBinding[UnderlyingCast(PredefinedShaderBinding::UboViewerData)] = textures.size() + uniformBlocks.size() + sharedUniformBlock.size();
		sharedUniformBlock.push_back(PredefinedViewerData::GetUniformBlock());

		// Shaders
		MaterialSettings::DefaultShaders defaultShaders;
		defaultShaders[UnderlyingCast(ShaderStageType::Fragment)] = Graphics::Instance()->GetRenderDevice().InstantiateShaderStage(Nz::ShaderStageType::Fragment, Nz::ShaderLanguage::NazaraBinary, r_fragmentShader, sizeof(r_fragmentShader));
		defaultShaders[UnderlyingCast(ShaderStageType::Vertex)] = Graphics::Instance()->GetRenderDevice().InstantiateShaderStage(Nz::ShaderStageType::Vertex, Nz::ShaderLanguage::NazaraBinary, r_vertexShader, sizeof(r_vertexShader));

		s_materialSettings = std::make_shared<MaterialSettings>(std::move(textures), std::move(uniformBlocks), std::move(sharedUniformBlock), predefinedBinding, std::move(defaultShaders));

		return true;
	}

	void BasicMaterial::Uninitialize()
	{
		s_materialSettings.reset();
	}

	std::shared_ptr<MaterialSettings> BasicMaterial::s_materialSettings;
	std::size_t BasicMaterial::s_uniformBlockIndex;
	BasicMaterial::TextureIndexes BasicMaterial::s_textureIndexes;
	BasicMaterial::UniformOffsets BasicMaterial::s_uniformOffsets;
}
