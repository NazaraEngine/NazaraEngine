// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <cstring>
#include <Nazara/Renderer/Debug.hpp>

inline NzRenderStates::NzRenderStates() :
dstBlend(nzBlendFunc_Zero),
srcBlend(nzBlendFunc_One),
faceFilling(nzFaceFilling_Fill),
faceCulling(nzFaceSide_Back),
depthFunc(nzRendererComparison_Less),
lineWidth(1.f),
pointSize(1.f)
{
	parameters[nzRendererParameter_Blend] = false;
	parameters[nzRendererParameter_ColorWrite] = true;
	parameters[nzRendererParameter_DepthBuffer] = false;
	parameters[nzRendererParameter_DepthWrite] = true;
	parameters[nzRendererParameter_FaceCulling] = false;
	parameters[nzRendererParameter_ScissorTest] = false;
	parameters[nzRendererParameter_StencilTest] = false;

	for (unsigned int i = 0; i < 2; ++i)
	{
		Face& face = (i == 0) ? backFace : frontFace;

		face.stencilCompare = nzRendererComparison_Always;
		face.stencilFail = nzStencilOperation_Keep;
		face.stencilMask = 0xFFFFFFFF;
		face.stencilPass = nzStencilOperation_Keep;
		face.stencilReference = 0;
		face.stencilZFail = nzStencilOperation_Keep;
	}
}

inline NzRenderStates::NzRenderStates(const NzRenderStates& states)
{
	std::memcpy(this, &states, sizeof(NzRenderStates));
}

inline NzRenderStates& NzRenderStates::operator=(const NzRenderStates& states)
{
	std::memcpy(this, &states, sizeof(NzRenderStates));

	return *this;
}

#include <Nazara/Renderer/DebugOff.hpp>
