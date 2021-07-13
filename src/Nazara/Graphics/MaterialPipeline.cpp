// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/MaterialPipeline.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Graphics/MaterialSettings.hpp>
#include <Nazara/Graphics/PhongLightingMaterial.hpp>
#include <Nazara/Graphics/UberShader.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::MaterialPipeline
	*
	* \brief Graphics class used to contains all rendering states that are not allowed to change individually on rendering devices
	*/

	/*!
	* \brief Retrieve (and generate if required) a pipeline instance using shader flags without applying it
	*
	* \param flags Shader flags
	*
	* \return Pipeline instance
	*/
	const std::shared_ptr<RenderPipeline>& MaterialPipeline::GetRenderPipeline(const std::vector<RenderPipelineInfo::VertexBufferData>& vertexBuffers) const
	{
		for (const auto& pipeline : m_renderPipelines)
		{
			const auto& pipelineInfo = pipeline->GetPipelineInfo();

			bool isEqual = std::equal(pipelineInfo.vertexBuffers.begin(), pipelineInfo.vertexBuffers.end(), vertexBuffers.begin(), [](const auto& v1, const auto& v2)
			{
				return v1.binding == v2.binding && v1.declaration == v2.declaration;
			});

			if (isEqual)
				return pipeline;
		}

		RenderPipelineInfo renderPipelineInfo;
		static_cast<RenderStates&>(renderPipelineInfo).operator=(m_pipelineInfo); // Not my proudest line

		renderPipelineInfo.pipelineLayout = m_pipelineInfo.settings->GetRenderPipelineLayout();

		for (const auto& shader : m_pipelineInfo.shaders)
		{
			if (shader.uberShader)
				renderPipelineInfo.shaderModules.push_back(shader.uberShader->Get(shader.enabledOptions));
		}

		renderPipelineInfo.vertexBuffers = vertexBuffers;

		return m_renderPipelines.emplace_back(Graphics::Instance()->GetRenderDevice()->InstantiateRenderPipeline(std::move(renderPipelineInfo)));
	}
	/*!
	* \brief Returns a reference to a MaterialPipeline built with MaterialPipelineInfo
	*
	* This function is using a cache, calling it multiples times with the same MaterialPipelineInfo will returns references to a single MaterialPipeline
	*
	* \param pipelineInfo Pipeline informations used to build/retrieve a MaterialPipeline object
	*/
	const std::shared_ptr<MaterialPipeline>& MaterialPipeline::Get(const MaterialPipelineInfo& pipelineInfo)
	{
		auto it = s_pipelineCache.find(pipelineInfo);
		if (it == s_pipelineCache.end())
			it = s_pipelineCache.insert(it, PipelineCache::value_type(pipelineInfo, std::make_shared<MaterialPipeline>(pipelineInfo, Token{})));

		return it->second;
	}

	bool MaterialPipeline::Initialize()
	{
		BasicMaterial::Initialize();
		PhongLightingMaterial::Initialize();

		return true;
	}

	void MaterialPipeline::Uninitialize()
	{
		s_pipelineCache.clear();
		PhongLightingMaterial::Uninitialize();
		BasicMaterial::Uninitialize();
	}

	MaterialPipeline::PipelineCache MaterialPipeline::s_pipelineCache;
}
