// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <NazaraUtils/MathUtils.hpp>
#include <functional>

namespace Nz
{
	bool operator==(const RenderStates& lhs, const RenderStates& rhs)
	{
		#define NazaraRenderStateMember(field) if (lhs.field != rhs.field) return false
		#define NazaraRenderStateBoolMember NazaraRenderStateMember
		#define NazaraRenderStateFloatMember(field, maxDiff) if (!NumberEquals(lhs.field, rhs.field, maxDiff)) return false

		NazaraRenderStateBoolMember(blending);
		NazaraRenderStateBoolMember(depthBias);
		NazaraRenderStateBoolMember(depthBuffer);
		NazaraRenderStateBoolMember(depthClamp);
		NazaraRenderStateBoolMember(scissorTest);
		NazaraRenderStateBoolMember(stencilTest);

		if (lhs.depthBuffer)
			NazaraRenderStateBoolMember(depthWrite);

		NazaraRenderStateMember(colorWriteMask);
		NazaraRenderStateMember(faceCulling);
		NazaraRenderStateMember(faceFilling);
		NazaraRenderStateMember(frontFace);
		NazaraRenderStateMember(primitiveMode);

		if (lhs.blending) //< Remember, at this time we know lhs.blending == rhs.blending
		{
			NazaraRenderStateMember(blend.dstAlpha);
			NazaraRenderStateMember(blend.dstColor);
			NazaraRenderStateMember(blend.modeAlpha);
			NazaraRenderStateMember(blend.modeColor);
			NazaraRenderStateMember(blend.srcAlpha);
			NazaraRenderStateMember(blend.srcColor);
		}

		if (lhs.depthBuffer)
			NazaraRenderStateMember(depthCompare);

		if (lhs.depthBias)
		{
			NazaraRenderStateMember(depthBiasConstantFactor);
			NazaraRenderStateMember(depthBiasSlopeFactor);
		}

		if (lhs.stencilTest)
		{
			NazaraRenderStateMember(stencilBack.compare);
			NazaraRenderStateMember(stencilBack.compareMask);
			NazaraRenderStateMember(stencilBack.depthFail);
			NazaraRenderStateMember(stencilBack.fail);
			NazaraRenderStateMember(stencilBack.pass);
			NazaraRenderStateMember(stencilBack.reference);
			NazaraRenderStateMember(stencilBack.writeMask);

			NazaraRenderStateMember(stencilFront.compare);
			NazaraRenderStateMember(stencilFront.compareMask);
			NazaraRenderStateMember(stencilFront.depthFail);
			NazaraRenderStateMember(stencilFront.fail);
			NazaraRenderStateMember(stencilFront.pass);
			NazaraRenderStateMember(stencilFront.reference);
			NazaraRenderStateMember(stencilFront.writeMask);
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
			#define NazaraRenderStateUInt8(member) Nz::HashCombine(seed, pipelineInfo.member)
			#define NazaraRenderStateUInt32(member) Nz::HashCombine(seed, pipelineInfo.member)

			NazaraRenderStateBool(blending);
			NazaraRenderStateBool(depthBias);
			NazaraRenderStateBool(depthBuffer);
			NazaraRenderStateBool(depthClamp);
			NazaraRenderStateBool(scissorTest);
			NazaraRenderStateBool(stencilTest);

			NazaraRenderStateBoolDep(depthBuffer, depthWrite);

			NazaraRenderStateUInt8(colorWriteMask);
			NazaraRenderStateEnum(faceCulling);
			NazaraRenderStateEnum(faceFilling);
			NazaraRenderStateEnum(frontFace);
			NazaraRenderStateEnum(primitiveMode);

			if (pipelineInfo.blending) //< we don't care about blending state if blending isn't enabled
			{
				NazaraRenderStateEnum(blend.dstAlpha);
				NazaraRenderStateEnum(blend.dstColor);
				NazaraRenderStateEnum(blend.modeAlpha);
				NazaraRenderStateEnum(blend.modeColor);
				NazaraRenderStateEnum(blend.srcAlpha);
				NazaraRenderStateEnum(blend.srcColor);
			}

			if (pipelineInfo.depthBuffer)
				NazaraRenderStateEnum(depthCompare);

			if (pipelineInfo.depthBias)
			{
				NazaraRenderStateFloat(depthBiasConstantFactor, 0.001f);
				NazaraRenderStateFloat(depthBiasSlopeFactor, 0.001f);
			}

			if (pipelineInfo.stencilTest) //< we don't care about stencil state if stencil isn't enabled
			{
				NazaraRenderStateEnum(stencilBack.compare);
				NazaraRenderStateUInt32(stencilBack.compareMask);
				NazaraRenderStateEnum(stencilBack.depthFail);
				NazaraRenderStateEnum(stencilBack.fail);
				NazaraRenderStateEnum(stencilBack.pass);
				NazaraRenderStateUInt32(stencilBack.reference);
				NazaraRenderStateUInt32(stencilBack.writeMask);

				NazaraRenderStateEnum(stencilFront.compare);
				NazaraRenderStateUInt32(stencilFront.compareMask);
				NazaraRenderStateEnum(stencilFront.depthFail);
				NazaraRenderStateEnum(stencilFront.fail);
				NazaraRenderStateEnum(stencilFront.pass);
				NazaraRenderStateUInt32(stencilFront.reference);
				NazaraRenderStateUInt32(stencilFront.writeMask);
			}

			NazaraRenderStateFloat(lineWidth, 0.001f);
			NazaraRenderStateFloat(pointSize, 0.001f);

			#undef NazaraRenderStateBool
			#undef NazaraRenderStateBoolDep
			#undef NazaraRenderStateEnum
			#undef NazaraRenderStateFloat
			#undef NazaraRenderStateUInt32

			Nz::HashCombine(seed, parameterHash);

			return seed;
		}
	};
}

