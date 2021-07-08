// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Renderer.hpp>
#include <functional>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline MaterialPipeline::MaterialPipeline(const MaterialPipelineInfo& pipelineInfo, Token) :
	m_pipelineInfo(pipelineInfo)
	{
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
		#define NazaraPipelineBoolMember NazaraPipelineMember

		NazaraPipelineBoolMember(depthSorting);
		NazaraPipelineBoolMember(hasVertexColor);
		NazaraPipelineBoolMember(reflectionMapping);
		NazaraPipelineBoolMember(shadowReceive);

		NazaraPipelineMember(settings);

		for (std::size_t i = 0; i < lhs.shaders.size(); ++i)
		{
			if (lhs.shaders[i].enabledOptions != rhs.shaders[i].enabledOptions)
				return false;

			if (lhs.shaders[i].uberShader != rhs.shaders[i].uberShader)
				return false;
		}

		#undef NazaraPipelineMember
		#undef NazaraPipelineBoolMember

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

			NazaraPipelineBoolMember(depthSorting);
			NazaraPipelineBoolMember(hasVertexColor);
			NazaraPipelineBoolMember(reflectionMapping);
			NazaraPipelineBoolMember(shadowReceive);

			NazaraPipelineMember(settings.get()); //< Hash pointer

			for (const auto& shader : pipelineInfo.shaders)
			{
				Nz::HashCombine(seed, shader.enabledOptions);
				Nz::HashCombine(seed, shader.uberShader.get());
			}

			#undef NazaraPipelineMember
			#undef NazaraPipelineBoolMember

			Nz::HashCombine(seed, parameterHash);

			return seed;
		}
	};
}

#include <Nazara/Graphics/DebugOff.hpp>
