// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/UberShaderInstance.hpp>
#include <functional>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline MaterialPipeline::MaterialPipeline(const MaterialPipelineInfo& pipelineInfo) :
	m_pipelineInfo(pipelineInfo)
	{
	}

	/*!
	* \brief Enable pipeline states for rendering
	*
	* \param flags Shader flags
	*/
	inline const MaterialPipeline::Instance& MaterialPipeline::Apply(UInt32 flags) const
	{
		const Instance& instance = GetInstance(flags);
		instance.uberInstance->Activate();

		Renderer::SetRenderStates(m_pipelineInfo);

		return instance;
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

	/*!
	* \brief Retrieve (and generate if required) a pipeline instance using shader flags without applying it
	*
	* \param flags Shader flags
	*
	* \return Pipeline instance
	*/
	inline const MaterialPipeline::Instance& MaterialPipeline::GetInstance(UInt32 flags) const
	{
		const Instance& instance = m_instances[flags];
		if (!instance.uberInstance)
			GenerateRenderPipeline(flags);

		return instance;
	}

	bool operator==(const MaterialPipelineInfo& lhs, const MaterialPipelineInfo& rhs)
	{
		if (!operator==(static_cast<const RenderStates&>(lhs), static_cast<const RenderStates&>(rhs)))
			return false;

		#define NazaraPipelineMember(field) if (lhs.field != rhs.field) return false
		#define NazaraPipelineBoolMember NazaraPipelineMember

		NazaraPipelineBoolMember(alphaTest);
		NazaraPipelineBoolMember(depthSorting);
		NazaraPipelineBoolMember(hasAlphaMap);
		NazaraPipelineBoolMember(hasDiffuseMap);
		NazaraPipelineBoolMember(hasEmissiveMap);
		NazaraPipelineBoolMember(hasHeightMap);
		NazaraPipelineBoolMember(hasNormalMap);
		NazaraPipelineBoolMember(hasSpecularMap);
		NazaraPipelineBoolMember(shadowReceive);

		NazaraPipelineMember(uberShader);

		#undef NazaraPipelineMember
		#undef NazaraPipelineBoolMember

		return true;
	}

	bool operator!=(const MaterialPipelineInfo& lhs, const MaterialPipelineInfo& rhs)
	{
		return !operator==(lhs, rhs);
	}

	/*!
	* \brief Creates a new MaterialPipeline from the arguments
	* \return A reference to the newly created material pipeline
	*
	* \param args Arguments for the material pipeline
	*/
	template<typename... Args>
	MaterialPipelineRef MaterialPipeline::New(Args&&... args)
	{
		std::unique_ptr<MaterialPipeline> object(new MaterialPipeline(std::forward<Args>(args)...));
		object->SetPersistent(false);
		return object.release();
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

			NazaraPipelineBoolMember(alphaTest);
			NazaraPipelineBoolMember(depthSorting);
			NazaraPipelineBoolMember(hasAlphaMap);
			NazaraPipelineBoolMember(hasDiffuseMap);
			NazaraPipelineBoolMember(hasEmissiveMap);
			NazaraPipelineBoolMember(hasHeightMap);
			NazaraPipelineBoolMember(hasNormalMap);
			NazaraPipelineBoolMember(hasSpecularMap);
			NazaraPipelineBoolMember(shadowReceive);

			NazaraPipelineMember(uberShader);

			#undef NazaraPipelineMember
			#undef NazaraPipelineBoolMember

			Nz::HashCombine(seed, parameterHash);

			return seed;
		}
	};
}

#include <Nazara/Graphics/DebugOff.hpp>
