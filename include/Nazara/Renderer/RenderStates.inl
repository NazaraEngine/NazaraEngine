// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <cstring>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline RenderStates::RenderStates() :
	dstBlend(BlendFunc_Zero),
	srcBlend(BlendFunc_One),
	faceFilling(FaceFilling_Fill),
	faceCulling(FaceSide_Back),
	depthFunc(RendererComparison_Less),
	lineWidth(1.f),
	pointSize(1.f)
	{
		parameters[RendererParameter_Blend] = false;
		parameters[RendererParameter_ColorWrite] = true;
		parameters[RendererParameter_DepthBuffer] = false;
		parameters[RendererParameter_DepthWrite] = true;
		parameters[RendererParameter_FaceCulling] = false;
		parameters[RendererParameter_ScissorTest] = false;
		parameters[RendererParameter_StencilTest] = false;

		for (unsigned int i = 0; i < 2; ++i)
		{
			Face& face = (i == 0) ? backFace : frontFace;

			face.stencilCompare = RendererComparison_Always;
			face.stencilFail = nzStencilOperation_Keep;
			face.stencilMask = 0xFFFFFFFF;
			face.stencilPass = nzStencilOperation_Keep;
			face.stencilReference = 0;
			face.stencilZFail = nzStencilOperation_Keep;
		}
	}

	inline RenderStates::RenderStates(const RenderStates& states)
	{
		std::memcpy(this, &states, sizeof(RenderStates));
	}

	inline RenderStates& RenderStates::operator=(const RenderStates& states)
	{
		std::memcpy(this, &states, sizeof(RenderStates));

		return *this;
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
