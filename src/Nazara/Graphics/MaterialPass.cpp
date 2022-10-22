// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/Graphics.hpp>
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
	m_pipelineUpdated(false)
	{
		m_pipelineInfo.pipelineLayout = std::move(settings.pipelineLayout);

		m_shaders.resize(settings.shaders.size());
		for (std::size_t i = 0; i < m_shaders.size(); ++i)
		{
			auto& shaderData = m_pipelineInfo.shaders.emplace_back();
			shaderData.uberShader = std::move(settings.shaders[i].uberShader);

			// TODO: Ensure pipeline layout compatibility using ShaderReflection

			m_shaders[i].onShaderUpdated.Connect(shaderData.uberShader->OnShaderUpdated, [this](UberShader*)
			{
				InvalidatePipeline();
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
