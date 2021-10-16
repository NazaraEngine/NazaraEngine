// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Graphics/UberShader.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderFrame.hpp>
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
