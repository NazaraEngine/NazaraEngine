// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
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
	m_pipelineUpdated(false)
	{
		m_pipelineInfo.settings = m_settings;

		const auto& shaders = m_settings->GetShaders();

		m_shaders.resize(shaders.size());
		for (std::size_t i = 0; i < m_shaders.size(); ++i)
		{
			auto& shaderData = m_pipelineInfo.shaders.emplace_back();
			shaderData.uberShader = shaders[i];

			m_shaders[i].onShaderUpdated.Connect(shaders[i]->OnShaderUpdated, [this](UberShader*)
			{
				InvalidatePipeline();
			});
		}

		const auto& textureSettings = m_settings->GetTextures();
		const auto& sharedUboSettings = m_settings->GetSharedUniformBlocks();
		const auto& uboSettings = m_settings->GetUniformBlocks();

		m_textures.resize(textureSettings.size());
		m_sharedUniformBuffers.resize(sharedUboSettings.size());

		m_uniformBuffers.reserve(uboSettings.size());
		for (const auto& uniformBufferInfo : uboSettings)
		{
			auto& uniformBuffer = m_uniformBuffers.emplace_back();

			uniformBuffer.buffer = Graphics::Instance()->GetRenderDevice()->InstantiateBuffer(BufferType::Uniform, uniformBufferInfo.blockSize, BufferUsage::Dynamic | BufferUsage::Write);

			assert(uniformBufferInfo.defaultValues.size() <= uniformBufferInfo.blockSize);

			uniformBuffer.data.resize(uniformBufferInfo.blockSize);
			std::memcpy(uniformBuffer.data.data(), uniformBufferInfo.defaultValues.data(), uniformBufferInfo.defaultValues.size());
		}
	}

	void MaterialPass::FillShaderBinding(std::vector<ShaderBinding::Binding>& bindings) const
	{
		const auto& textureSettings = m_settings->GetTextures();
		const auto& sharedUboSettings = m_settings->GetSharedUniformBlocks();
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

		// Shared UBO
		for (std::size_t i = 0; i < m_sharedUniformBuffers.size(); ++i)
		{
			const auto& sharedUboSlot = m_sharedUniformBuffers[i];
			if (!sharedUboSlot.bufferView)
				continue;

			const auto& sharedUboSetting = sharedUboSettings[i];

			bindings.push_back({
				sharedUboSetting.bindingIndex,
				ShaderBinding::UniformBufferBinding {
					sharedUboSlot.bufferView.GetBuffer(), sharedUboSlot.bufferView.GetOffset(), sharedUboSlot.bufferView.GetSize()
				}
			});
		}

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

	void MaterialPass::Update(RenderFrame& renderFrame, CommandBufferBuilder& builder)
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
	}

	void MaterialPass::UpdatePipeline() const
	{
		m_pipelineInfo.optionCount = 0;

		const auto& options = m_settings->GetOptions();
		for (std::size_t optionIndex = 0; optionIndex < options.size(); ++optionIndex)
		{
			if (!std::holds_alternative<nzsl::Ast::NoValue>(m_optionValues[optionIndex]))
			{
				auto& optionValue = m_pipelineInfo.optionValues[m_pipelineInfo.optionCount];
				optionValue.hash = options[optionIndex].hash;
				optionValue.value = m_optionValues[optionIndex];

				m_pipelineInfo.optionCount++;
			}
		}

		// make option values consistent (required for hash/equality)
		std::sort(m_pipelineInfo.optionValues.begin(), m_pipelineInfo.optionValues.begin() + m_pipelineInfo.optionCount, [](const auto& lhs, const auto& rhs) { return lhs.hash < rhs.hash; });

		m_pipeline = MaterialPipeline::Get(m_pipelineInfo);
		m_pipelineUpdated = true;
	}
}
