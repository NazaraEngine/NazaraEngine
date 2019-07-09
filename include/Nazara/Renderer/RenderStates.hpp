// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERSTATES_HPP
#define NAZARA_RENDERSTATES_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Utility/Enums.hpp>

namespace Nz
{
	struct RenderStates
	{
		BlendFunc dstBlend = BlendFunc_Zero;
		BlendFunc srcBlend = BlendFunc_One;
		FaceFilling faceFilling = FaceFilling_Fill;
		FaceSide cullingSide = FaceSide_Back;
		RendererComparison depthFunc = RendererComparison_Less;

		struct
		{
			RendererComparison back  = RendererComparison_Always;
			RendererComparison front = RendererComparison_Always;
		} stencilCompare;

		struct
		{
			UInt32 back  = 0xFFFFFFFF;
			UInt32 front = 0xFFFFFFFF;
		} stencilCompareMask;

		struct
		{
			StencilOperation back  = StencilOperation_Keep;
			StencilOperation front = StencilOperation_Keep;
		} stencilDepthFail;

		struct
		{
			StencilOperation back  = StencilOperation_Keep;
			StencilOperation front = StencilOperation_Keep;
		} stencilFail;

		struct
		{
			StencilOperation back  = StencilOperation_Keep;
			StencilOperation front = StencilOperation_Keep;
		} stencilPass;

		struct
		{
			UInt32 back  = 0U;
			UInt32 front = 0U;
		} stencilReference;

		struct
		{
			UInt32 back  = 0xFFFFFFFF;
			UInt32 front = 0xFFFFFFFF;
		} stencilWriteMask;

		bool blending    = false;
		bool colorWrite  = true;
		bool depthBuffer = false;
		bool depthWrite  = true;
		bool faceCulling = false;
		bool scissorTest = false;
		bool stencilTest = false;

		float lineWidth = 1.f;
		float pointSize = 1.f;
	};

	inline bool operator==(const RenderStates& lhs, const RenderStates& rhs);
}

#include <Nazara/Renderer/RenderStates.inl>

#endif // NAZARA_RENDERSTATES_HPP
