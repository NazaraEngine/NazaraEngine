// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/BasicMaterialPass.hpp>
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
	MaterialPass::MaterialPass(Settings&& settings) :
	m_isEnabled(true),
	m_pipelineUpdated(false)
	{
		m_pipelineInfo.pipelineLayout = std::move(settings.pipelineLayout);

		m_shaders.resize(settings.shaders.size());
		for (std::size_t i = 0; i < m_shaders.size(); ++i)
		{
			auto& shaderData = m_pipelineInfo.shaders.emplace_back();
			shaderData.uberShader = std::move(settings.shaders[i].uberShader);

			m_shaders[i].onShaderUpdated.Connect(shaderData.uberShader->OnShaderUpdated, [this](UberShader*)
			{
				InvalidatePipeline();
			});
		}

		m_textures.resize(settings.textures.size());
		for (std::size_t i = 0; i < m_textures.size(); ++i)
		{
			auto&& textureInfo = std::move(settings.textures[i]);

			m_textures[i].bindingIndex = textureInfo.bindingIndex;
			m_textures[i].samplerInfo = std::move(textureInfo.samplerInfo);
			m_textures[i].texture = std::move(textureInfo.texture);
		}

		m_uniformBuffers.resize(settings.uniformBuffers.size());
		for (std::size_t i = 0; i < m_uniformBuffers.size(); ++i)
		{
			auto&& uboInfo = std::move(settings.uniformBuffers[i]);

			m_uniformBuffers[i].bindingIndex = uboInfo.bindingIndex;
			m_uniformBuffers[i].buffer = std::move(uboInfo.buffer);
			m_uniformBuffers[i].bufferView = std::move(uboInfo.bufferView);
		}
	}

	void MaterialPass::FillShaderBinding(std::vector<ShaderBinding::Binding>& bindings) const
	{
		// Textures
		for (std::size_t i = 0; i < m_textures.size(); ++i)
		{
			const auto& textureSlot = m_textures[i];

			if (!textureSlot.sampler)
			{
				TextureSamplerCache& samplerCache = Graphics::Instance()->GetSamplerCache();
				textureSlot.sampler = samplerCache.Get(textureSlot.samplerInfo);
			}

			bindings.push_back({
				textureSlot.bindingIndex,
				ShaderBinding::TextureBinding {
					textureSlot.texture.get(), textureSlot.sampler.get()
				}
			});
		}

		// UBO
		for (std::size_t i = 0; i < m_uniformBuffers.size(); ++i)
		{
			const auto& uboInfo = m_uniformBuffers[i];

			bindings.push_back({
				uboInfo.bindingIndex,
				ShaderBinding::UniformBufferBinding {
					uboInfo.bufferView.GetBuffer(), uboInfo.bufferView.GetOffset(), uboInfo.bufferView.GetSize()
				}
			});
		}
	}

	void MaterialPass::UpdatePipeline() const
	{
		NazaraAssert(m_optionValues.size() < m_pipelineInfo.optionValues.size(), "too many options");

		m_pipelineInfo.optionCount = m_optionValues.size();

		for (auto&& [optionHash, value] : m_optionValues)
		{
			auto& optionValue = m_pipelineInfo.optionValues[m_pipelineInfo.optionCount];
			optionValue.hash = optionHash;
			optionValue.value = value;
		}

		// make option values consistent (required for hash/equality)
		std::sort(m_pipelineInfo.optionValues.begin(), m_pipelineInfo.optionValues.begin() + m_pipelineInfo.optionCount, [](const auto& lhs, const auto& rhs) { return lhs.hash < rhs.hash; });

		m_pipeline = MaterialPipeline::Get(m_pipelineInfo);
		m_pipelineUpdated = true;
	}
}
