// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Graphics/UberShader.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderFrame.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <Nazara/Utility/MaterialData.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::Material
	* \brief Graphics class that represents a material
	*/

	/*!
	* \brief Constructs a Material object with default states
	*
	* \see Reset
	*/
	MaterialPass::MaterialPass(std::shared_ptr<const MaterialSettings> settings) :
	m_settings(std::move(settings)),
	m_forceCommandBufferRegeneration(false),
	m_pipelineUpdated(false)
	{
		m_pipelineInfo.settings = m_settings;

		const auto& shaders = m_settings->GetShaders();
		for (const auto& shader : shaders)
		{
			auto& shaderData = m_pipelineInfo.shaders.emplace_back();
			shaderData.uberShader = shader;
		}

		const auto& textureSettings = m_settings->GetTextures();
		const auto& uboSettings = m_settings->GetUniformBlocks();

		m_textures.resize(textureSettings.size());

		m_uniformBuffers.reserve(uboSettings.size());
		for (const auto& uniformBufferInfo : uboSettings)
		{
			auto& uniformBuffer = m_uniformBuffers.emplace_back();

			uniformBuffer.buffer = Graphics::Instance()->GetRenderDevice()->InstantiateBuffer(BufferType::Uniform);
			if (!uniformBuffer.buffer->Initialize(uniformBufferInfo.blockSize, BufferUsage::Dynamic))
				throw std::runtime_error("failed to initialize UBO memory");

			assert(uniformBufferInfo.defaultValues.size() <= uniformBufferInfo.blockSize);

			uniformBuffer.data.resize(uniformBufferInfo.blockSize);
			std::memcpy(uniformBuffer.data.data(), uniformBufferInfo.defaultValues.data(), uniformBufferInfo.defaultValues.size());
		}
	}

	void MaterialPass::FillShaderBinding(std::vector<ShaderBinding::Binding>& bindings) const
	{
		const auto& textureSettings = m_settings->GetTextures();
		const auto& uboSettings = m_settings->GetUniformBlocks();

		// Textures
		for (std::size_t i = 0; i < m_textures.size(); ++i)
		{
			const auto& textureSetting = textureSettings[i];
			const auto& textureSlot = m_textures[i];

			if (!textureSlot.sampler)
			{
				TextureSamplerCache& samplerCache = Graphics::Instance()->GetSamplerCache();
				textureSlot.sampler = samplerCache.Get(textureSlot.samplerInfo);
			}

			//TODO: Use "missing" texture
			Texture* texture = textureSlot.texture.get();
			if (!texture)
			{
				const auto& defaultTextures = Graphics::Instance()->GetDefaultTextures();
				texture = defaultTextures.whiteTextures[UnderlyingCast(textureSetting.type)].get();
			}

			bindings.push_back({
				textureSetting.bindingIndex,
				ShaderBinding::TextureBinding {
					texture, textureSlot.sampler.get()
				}
			});
		}

		// Shared UBO (TODO)

		// Owned UBO
		for (std::size_t i = 0; i < m_uniformBuffers.size(); ++i)
		{
			const auto& uboSetting = uboSettings[i];
			const auto& uboSlot = m_uniformBuffers[i];

			bindings.push_back({
				uboSetting.bindingIndex,
				ShaderBinding::UniformBufferBinding {
					uboSlot.buffer.get(), 0, uboSlot.buffer->GetSize()
				}
			});
		}
	}

	bool MaterialPass::Update(RenderFrame& renderFrame, CommandBufferBuilder& builder)
	{
		UploadPool& uploadPool = renderFrame.GetUploadPool();

		for (auto& ubo : m_uniformBuffers)
		{
			if (ubo.dataInvalidated)
			{
				auto& allocation = uploadPool.Allocate(ubo.data.size());
				std::memcpy(allocation.mappedPtr, ubo.data.data(), ubo.data.size());

				builder.CopyBuffer(allocation, ubo.buffer.get());

				ubo.dataInvalidated = false;
			}
		}

		bool shouldRegenerateCommandBuffer = m_forceCommandBufferRegeneration;
		m_forceCommandBufferRegeneration = false;

		return shouldRegenerateCommandBuffer;
	}

	void MaterialPass::UpdatePipeline() const
	{
		for (auto& shader : m_pipelineInfo.shaders)
			shader.optionValues.fill(ShaderAst::NoValue{});

		const auto& options = m_settings->GetOptions();
		for (std::size_t optionIndex = 0; optionIndex < options.size(); ++optionIndex)
		{
			const auto& option = options[optionIndex];
			assert(option.optionIndexByShader.size() <= m_pipelineInfo.shaders.size());

			for (std::size_t shaderIndex = 0; shaderIndex < option.optionIndexByShader.size(); ++shaderIndex)
			{
				if (!option.optionIndexByShader[shaderIndex].has_value())
					continue;

				m_pipelineInfo.shaders[shaderIndex].optionValues[optionIndex] = m_optionValues[optionIndex];
			}
		}

		m_pipeline = MaterialPipeline::Get(m_pipelineInfo);
		m_pipelineUpdated = true;
	}
}
