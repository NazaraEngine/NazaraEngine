// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Renderer/Renderer.hpp>
#include <NazaraUtils/Hash.hpp>
#include <functional>

namespace Nz
{
	inline MaterialPipeline::MaterialPipeline(const MaterialPipelineInfo& pipelineInfo, Token) :
	m_pipelineInfo(pipelineInfo)
	{
		m_uberShaderEntries.resize(m_pipelineInfo.shaders.size());
		for (std::size_t i = 0; i < m_uberShaderEntries.size(); ++i)
		{
			m_uberShaderEntries[i].onShaderUpdated.Connect(m_pipelineInfo.shaders[i].uberShader->OnShaderUpdated, [this](UberShader*)
			{
				// Clear cache
				m_renderPipelines.clear();
			});
		}
	}

	/*!
	* \brief Retrieve a MaterialPipelineInfo object describing this pipeline
	*
	* \return Pipeline informations
	*/
	const MaterialPipelineInfo& MaterialPipeline::GetInfo() const
	{
		return m_pipelineInfo;
	}

	bool operator==(const MaterialPipelineInfo& lhs, const MaterialPipelineInfo& rhs)
	{
		if (!operator==(static_cast<const RenderStates&>(lhs), static_cast<const RenderStates&>(rhs)))
			return false;

		#define NazaraPipelineMember(field) if (lhs.field != rhs.field) return false

		for (std::size_t i = 0; i < lhs.shaders.size(); ++i)
		{
			if (lhs.shaders[i].uberShader != rhs.shaders[i].uberShader)
				return false;
		}

		for (std::size_t i = 0; i < lhs.optionValues.size(); ++i)
		{
			if (lhs.optionValues[i].hash != rhs.optionValues[i].hash)
				return false;

			if (lhs.optionValues[i].value != rhs.optionValues[i].value)
				return false;
		}

		#undef NazaraPipelineMember

		return true;
	}

	bool operator!=(const MaterialPipelineInfo& lhs, const MaterialPipelineInfo& rhs)
	{
		return !operator==(lhs, rhs);
	}
}

namespace std
{
	template<>
	struct hash<Nz::MaterialPipelineInfo>
	{
		size_t operator()(const Nz::MaterialPipelineInfo& pipelineInfo) const
		{
			hash<Nz::RenderStates> parentHash;

			std::size_t seed = parentHash(pipelineInfo);

			Nz::UInt16 parameterHash = 0;
			Nz::UInt16 parameterIndex = 0;

			#define NazaraPipelineMember(member) Nz::HashCombine(seed, pipelineInfo.member)
			#define NazaraPipelineBoolMember(member) parameterHash |= ((pipelineInfo.member) ? 1U : 0U) << (parameterIndex++)

			for (std::size_t i = 0; i < pipelineInfo.optionValues.size(); ++i)
			{
				Nz::HashCombine(seed, pipelineInfo.optionValues[i].hash);
				Nz::HashCombine(seed, pipelineInfo.optionValues[i].value);
			}

			for (const auto& shader : pipelineInfo.shaders)
				Nz::HashCombine(seed, shader.uberShader.get());

			NazaraUnused(parameterIndex);

			#undef NazaraPipelineMember
			#undef NazaraPipelineBoolMember

			Nz::HashCombine(seed, parameterHash);

			return seed;
		}
	};
}
