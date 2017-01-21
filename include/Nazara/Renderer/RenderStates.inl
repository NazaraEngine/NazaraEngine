// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <functional>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	bool operator==(const RenderStates& lhs, const RenderStates& rhs)
	{
		#define NazaraRenderStateMember(field) if (lhs.field != rhs.field) return false
		#define NazaraRenderStateBoolMember NazaraRenderStateMember
		#define NazaraRenderStateFloatMember(field, maxDiff) if (!NumberEquals(lhs.field, rhs.field, maxDiff)) return false

		NazaraRenderStateBoolMember(blending);
		NazaraRenderStateBoolMember(colorWrite);
		NazaraRenderStateBoolMember(depthBuffer);
		NazaraRenderStateBoolMember(faceCulling);
		NazaraRenderStateBoolMember(scissorTest);
		NazaraRenderStateBoolMember(stencilTest);

		if (lhs.depthBuffer)
			NazaraRenderStateBoolMember(depthWrite);

		NazaraRenderStateMember(faceFilling);

		if (lhs.blending) //< Remember, at this time we know lhs.blending == rhs.blending
		{
			NazaraRenderStateMember(dstBlend);
			NazaraRenderStateMember(srcBlend);
		}

		if (lhs.depthBuffer)
			NazaraRenderStateMember(depthFunc);

		if (lhs.faceCulling)
			NazaraRenderStateMember(cullingSide);

		if (lhs.stencilTest)
		{
			NazaraRenderStateMember(stencilCompare.back);
			NazaraRenderStateMember(stencilCompare.front);
			NazaraRenderStateMember(stencilCompareMask.back);
			NazaraRenderStateMember(stencilCompareMask.front);
			NazaraRenderStateMember(stencilDepthFail.back);
			NazaraRenderStateMember(stencilDepthFail.front);
			NazaraRenderStateMember(stencilFail.back);
			NazaraRenderStateMember(stencilFail.front);
			NazaraRenderStateMember(stencilPass.back);
			NazaraRenderStateMember(stencilPass.front);
			NazaraRenderStateMember(stencilReference.back);
			NazaraRenderStateMember(stencilReference.front);
			NazaraRenderStateMember(stencilWriteMask.back);
			NazaraRenderStateMember(stencilWriteMask.front);
		}

		NazaraRenderStateFloatMember(lineWidth, 0.001f);
		NazaraRenderStateFloatMember(pointSize, 0.001f);

		#undef NazaraRenderStateMember
		#undef NazaraRenderStateBoolMember
		#undef NazaraRenderStateFloatMember

		return true;
	}
}

namespace std
{
	template<>
	struct hash<Nz::RenderStates>
	{
		size_t operator()(const Nz::RenderStates& pipelineInfo) const
		{
			std::size_t seed = 0;

			Nz::UInt8 parameterHash = 0;
			Nz::UInt8 parameterIndex = 0;

			#define NazaraRenderStateBool(member) parameterHash |= ((pipelineInfo.member) ? 1U : 0U) << (parameterIndex++)
			#define NazaraRenderStateBoolDep(dependency, member) parameterHash |= ((pipelineInfo.dependency && pipelineInfo.member) ? 1U : 0U) << (parameterIndex++)
			#define NazaraRenderStateEnum(member) Nz::HashCombine(seed, static_cast<Nz::UInt8>(pipelineInfo.member))
			#define NazaraRenderStateFloat(member, maxDiff) Nz::HashCombine(seed, std::floor(pipelineInfo.member / maxDiff) * maxDiff)

			NazaraRenderStateBool(blending);
			NazaraRenderStateBool(colorWrite);
			NazaraRenderStateBool(depthBuffer);
			NazaraRenderStateBool(faceCulling);
			NazaraRenderStateBool(scissorTest);
			NazaraRenderStateBool(stencilTest);

			NazaraRenderStateBoolDep(depthBuffer, depthWrite);

			NazaraRenderStateEnum(faceFilling);

			if (pipelineInfo.blending) //< Remember, at this time we know lhs.blending == rhs.blending
			{
				NazaraRenderStateEnum(dstBlend);
				NazaraRenderStateEnum(srcBlend);
			}

			if (pipelineInfo.depthBuffer)
				NazaraRenderStateEnum(depthFunc);

			if (pipelineInfo.faceCulling)
				NazaraRenderStateEnum(cullingSide);

			if (pipelineInfo.stencilTest)
			{
				NazaraRenderStateEnum(stencilCompare.back);
				NazaraRenderStateEnum(stencilCompare.front);
				NazaraRenderStateEnum(stencilCompareMask.back);
				NazaraRenderStateEnum(stencilCompareMask.front);
				NazaraRenderStateEnum(stencilDepthFail.back);
				NazaraRenderStateEnum(stencilDepthFail.front);
				NazaraRenderStateEnum(stencilFail.back);
				NazaraRenderStateEnum(stencilFail.front);
				NazaraRenderStateEnum(stencilPass.back);
				NazaraRenderStateEnum(stencilPass.front);
				NazaraRenderStateEnum(stencilReference.back);
				NazaraRenderStateEnum(stencilReference.front);
				NazaraRenderStateEnum(stencilWriteMask.back);
				NazaraRenderStateEnum(stencilWriteMask.front);
			}

			NazaraRenderStateFloat(lineWidth, 0.001f);
			NazaraRenderStateFloat(pointSize, 0.001f);

			#undef NazaraRenderStateBool
			#undef NazaraRenderStateBoolDep
			#undef NazaraRenderStateEnum
			#undef NazaraRenderStateFloat

			Nz::HashCombine(seed, parameterHash);

			return seed;
		}
	};
}

#include <Nazara/Renderer/DebugOff.hpp>
