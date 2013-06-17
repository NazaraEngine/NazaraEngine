// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <cstring>
#include <Nazara/Renderer/Debug.hpp>

inline NzRenderStates::NzRenderStates() :
dstBlend(nzBlendFunc_Zero),
srcBlend(nzBlendFunc_One),
faceCulling(nzFaceCulling_Back),
faceFilling(nzFaceFilling_Fill),
depthFunc(nzRendererComparison_Less),
stencilCompare(nzRendererComparison_Always),
stencilFail(nzStencilOperation_Keep),
stencilPass(nzStencilOperation_Keep),
stencilZFail(nzStencilOperation_Keep),
stencilMask(0xFFFFFFFF),
lineWidth(1.f),
pointSize(1.f),
stencilReference(0)
{
	parameters[nzRendererParameter_Blend] = false;
	parameters[nzRendererParameter_ColorWrite] = true;
	parameters[nzRendererParameter_DepthBuffer] = false;
	parameters[nzRendererParameter_DepthWrite] = true;
	parameters[nzRendererParameter_FaceCulling] = false;
	parameters[nzRendererParameter_ScissorTest] = false;
	parameters[nzRendererParameter_StencilTest] = false;
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
