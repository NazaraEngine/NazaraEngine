// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERSTATES_HPP
#define NAZARA_RENDERSTATES_HPP

#include <Nazara/Renderer/Enums.hpp>

struct NzRenderStates
{
	NzRenderStates();
	NzRenderStates(const NzRenderStates& states);
	~NzRenderStates() = default;

	NzRenderStates& operator=(const NzRenderStates& states);

	nzBlendFunc dstBlend;
	nzBlendFunc srcBlend;
	nzFaceFilling faceFilling;
	nzFaceSide faceCulling;
	nzRendererComparison depthFunc;
	nzRendererComparison stencilCompare;
	nzStencilOperation stencilFail;
	nzStencilOperation stencilPass;
	nzStencilOperation stencilZFail;
	nzUInt32 stencilMask;
	bool parameters[nzRendererParameter_Max+1];
	float lineWidth;
	float pointSize;
	unsigned int stencilReference;
};

#include <Nazara/Renderer/RenderStates.inl>

#endif // NAZARA_RENDERSTATES_HPP
