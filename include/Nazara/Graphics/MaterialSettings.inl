// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/MaterialSettings.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline MaterialSettings::MaterialSettings() :
	MaterialSettings({}, {}, {}, { InvalidIndex }, {})
	{
	}

	inline MaterialSettings::MaterialSettings(std::vector<Texture> textures, std::vector<UniformBlock> uniformBlocks, std::vector<SharedUniformBlock> sharedUniformBlocks, const PredefinedBinding& predefinedBindings, DefaultShaders defaultShaders) :
	m_sharedUniformBlocks(std::move(sharedUniformBlocks)),
	m_textures(std::move(textures)),
	m_uniformBlocks(std::move(uniformBlocks)),
	m_defaultShaders(std::move(defaultShaders)),
	m_predefinedBinding(predefinedBindings)
	{
		RenderPipelineLayoutInfo info;

		unsigned int bindingIndex = 0;

		for (const Texture& textureInfo : m_textures)
		{
			info.bindings.push_back({
				//textureInfo.bindingPoint,
				ShaderBindingType::Texture,
				ShaderStageType_All,
				bindingIndex++
			});
		}

		for (const UniformBlock& ubo : m_uniformBlocks)
		{
			info.bindings.push_back({
				//ubo.bindingPoint,
				ShaderBindingType::UniformBuffer,
				ShaderStageType_All,
				bindingIndex++
			});
		}

		for (const SharedUniformBlock& ubo : m_sharedUniformBlocks)
		{
			info.bindings.push_back({
				//ubo.bindingPoint,
				ShaderBindingType::UniformBuffer,
				ShaderStageType_All,
				bindingIndex++
			});
		}

		m_pipelineLayout = Graphics::Instance()->GetRenderDevice().InstantiateRenderPipelineLayout(std::move(info));
	}

	inline const std::shared_ptr<ShaderStage>& MaterialSettings::GetDefaultShader(ShaderStageType stage) const
	{
		return m_defaultShaders[UnderlyingCast(stage)];
	}

	inline auto MaterialSettings::GetDefaultShaders() const -> const DefaultShaders&
	{
		return m_defaultShaders;
	}

	inline std::size_t MaterialSettings::GetPredefinedBindingIndex(PredefinedShaderBinding binding) const
	{
		return m_predefinedBinding[UnderlyingCast(binding)];
	}

	inline const std::shared_ptr<RenderPipelineLayout>& MaterialSettings::GetRenderPipelineLayout() const
	{
		return m_pipelineLayout;
	}

	inline auto MaterialSettings::GetSharedUniformBlocks() const -> const std::vector<SharedUniformBlock>&
	{
		return m_sharedUniformBlocks;
	}

	inline std::size_t MaterialSettings::GetSharedUniformBlockIndex(const std::string_view& name) const
	{
		for (std::size_t i = 0; i < m_sharedUniformBlocks.size(); ++i)
		{
			if (m_sharedUniformBlocks[i].name == name)
				return i;
		}

		return InvalidIndex;
	}

	inline auto MaterialSettings::GetTextures() const -> const std::vector<Texture>&
	{
		return m_textures;
	}

	inline std::size_t MaterialSettings::GetSharedUniformBlockVariableOffset(std::size_t uniformBlockIndex, const std::string_view& name) const
	{
		assert(uniformBlockIndex < m_sharedUniformBlocks.size());

		const std::vector<UniformVariable>& variables = m_sharedUniformBlocks[uniformBlockIndex].uniforms;
		for (std::size_t i = 0; i < variables.size(); ++i)
		{
			if (variables[i].name == name)
				return i;
		}

		return InvalidIndex;
	}

	inline std::size_t MaterialSettings::GetTextureIndex(const std::string_view& name) const
	{
		for (std::size_t i = 0; i < m_textures.size(); ++i)
		{
			if (m_textures[i].name == name)
				return i;
		}

		return InvalidIndex;
	}

	inline auto MaterialSettings::GetUniformBlocks() const -> const std::vector<UniformBlock>&
	{
		return m_uniformBlocks;
	}

	inline std::size_t MaterialSettings::GetUniformBlockIndex(const std::string_view& name) const
	{
		for (std::size_t i = 0; i < m_uniformBlocks.size(); ++i)
		{
			if (m_uniformBlocks[i].name == name)
				return i;
		}

		return InvalidIndex;
	}

	inline std::size_t MaterialSettings::GetUniformBlockVariableOffset(std::size_t uniformBlockIndex, const std::string_view& name) const
	{
		assert(uniformBlockIndex < m_uniformBlocks.size());

		const std::vector<UniformVariable>& variables = m_uniformBlocks[uniformBlockIndex].uniforms;
		for (std::size_t i = 0; i < variables.size(); ++i)
		{
			if (variables[i].name == name)
				return i;
		}

		return InvalidIndex;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
